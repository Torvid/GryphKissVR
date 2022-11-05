
#include "macros.shader"

#define ShaderName defaultlit

#define Parameters(X) \
	X(float3, Color)\
	X(sampler2D, texM1)\
	X(sampler2D, texM2)\

#define addBoneTransforms 1

#include "shaderMacros.cpp"
#include "shaderMacros.shader"

// Inputs
//uniform sampler2D texM1; // Color, Roughness
//uniform sampler2D texM2; // Normal, AO, Metallic
//uniform sampler2D texM3; // Alpha
//
//uniform sampler2D texM1Extra; // Color, Roughness
//uniform sampler2D texM2Extra; // Normal, AO, Metallic
//uniform sampler2D texM3Extra; // Alpha
//
//uniform float3 color;
//#if Cpp
//#else
//uniform float4x4 boneTransforms[200];
//#endif

#ifdef vertexShader

in float3 VertexPos;
in float3 VertexNormal;
in float3 VertexTangent;
in float2 VertexUV0;
in float2 VertexUV1;
in int MaterialID;
in float4 BoneWeights;
in int4 BoneIndexes;

out float3 PSVertexPos;
out float3 PSVertexNormal;
out float3 PSVertexTangent;
out float3 PSVertexBitangent;
out float2 PSVertexUV;
out int PSMaterialID;

void main()
{
	float3 pos = VertexPos;
	float3 normal = VertexNormal;
	float3 tangent = VertexTangent;
	if (IsSkinned > 0.5)
	{
		float4x4 skinning = float4x4(0);
		//float sum = BoneWeights.x + BoneWeights.y + BoneWeights.z + BoneWeights.w;
		float sum = float(BoneWeights.x + BoneWeights.y + BoneWeights.z + BoneWeights.w);
		
		skinning += boneTransforms[BoneIndexes.x] * (BoneWeights.x);
		skinning += boneTransforms[BoneIndexes.y] * (BoneWeights.y);
		skinning += boneTransforms[BoneIndexes.z] * (BoneWeights.z);
		skinning += boneTransforms[BoneIndexes.w] * (BoneWeights.w);
	
		pos = (float4(VertexPos, 1.0) * skinning).xyz;
		normal = (float4(VertexNormal, 0.0) * skinning).xyz;
		tangent = (float4(VertexTangent, 0.0) * skinning).xyz;
	}

	gl_Position = ModelViewProjection * float4(pos, 1.0);

	// Global stuff
	float3 worldNormal = (WorldToModel * float4(normal, 0.0f)).xyz;
	float3 worldTangent = (WorldToModel * float4(tangent, 0.0f)).xyz;
	float3 worldPos = (WorldToModel * float4(pos, 1.0)).xyz;

	worldNormal = float3(worldNormal.x, -worldNormal.z, worldNormal.y);
	worldTangent = float3(worldTangent.x, -worldTangent.z, worldTangent.y);
	worldPos = float3(worldPos.x, -worldPos.z, worldPos.y);

	PSVertexPos = worldPos;
	PSVertexNormal = worldNormal;
	PSVertexTangent = worldTangent;
	PSVertexBitangent = cross(VertexTangent, worldNormal);
	PSVertexUV = VertexUV0;
	PSMaterialID = MaterialID;
}

#endif


#ifdef pixelShader

float3 deriveTangentBasis(float3 VertexNormalWS, float3 AbsoluteWorldPosition, float2 UVs, float3 TextureNormal)
{
	// get edge vectors of the pixel triangle
	float3 dp1 = dFdx(AbsoluteWorldPosition);
	float3 dp2 = dFdy(AbsoluteWorldPosition);
	float2 duv1 = dFdx(UVs);
	float2 duv2 = dFdy(UVs);

	// solve the linear system
	float3 dp2perp = cross(dp2, VertexNormalWS);
	float3 dp1perp = cross(VertexNormalWS, dp1);
	float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	float3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	float invmax = 1.0 / sqrt(max(dot(T, T), dot(B, B)));
	T = (T * invmax);
	B = (B * invmax);
	float3 N = VertexNormalWS;

	return T * TextureNormal.x + B * -TextureNormal.y + TextureNormal.z * N;
}

in float3 PSVertexPos;
in float3 PSVertexNormal;
in float3 PSVertexTangent;
in float3 PSVertexBitangent;
in float2 PSVertexUV;
flat in int PSMaterialID;

out float4 FragColor;

void main()
{
	float2 UV = PSVertexUV;
	float3 worldPos = PSVertexPos;
	
	float4 M1 = Sample(texM1, UV);
	float4 M2 = Sample(texM2, UV);

	// Tangent-Space Normal mapping
	M2.rg = M2.rg * 2.0 - 1.0;

	float3 worldNormal = PSVertexNormal *1.0 + M2.r * PSVertexTangent + M2.g * PSVertexBitangent;
	//float3 worldNormal = deriveTangentBasis(PSVertexNormal, worldPos, UV, float3(M2.rg, 1));

	// Lighting
	float NdotL = dot(worldNormal, float3(0, 1, 1));
	float halflambert = NdotL * 0.5 + 0.5;
	
	float3 lighting = float3(saturate(NdotL)) + float3(0.4, 0.3, 0.2);
	
	FragColor.rgb = M1.rgb * lighting;
	
	if (M2.a < 0.5)
		discard;
	
	// Ensure it never goes below 0, headset colors are abs'd?
	FragColor.rgb = max(FragColor.rgb, float3(0, 0, 0));
}
#endif
