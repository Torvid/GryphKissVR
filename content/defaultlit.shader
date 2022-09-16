
#include "macros.shader"

#define ShaderName defaultlit

#define Parameters(X) \
	X(float3, Color)\
	X(sampler2D, texM1)\
	X(sampler2D, texM2)\
	X(sampler2D, texM3)\
	X(sampler2D, texM1Extra)\
	X(sampler2D, texM2Extra)\
	X(sampler2D, texM3Extra)

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
in float2 VertexUV0;
in float2 VertexUV1;
in int MaterialID;
in float4 BoneWeights;
in float4 BoneIndexes;

out float3 PSVertexPos;
out float3 PSVertexNormal;
out float2 PSVertexUV;
out float3 PSVertexColor;
out int PSMaterialID;

void main()
{
	float3 pos = VertexPos;
	float3 normal = VertexNormal;
	if (IsSkinned > 0.5)
	{
		float4x4 skinning = float4x4(0);
		float sum = BoneWeights.x + BoneWeights.y + BoneWeights.z + BoneWeights.w;
	
		skinning += boneTransforms[int(round(BoneIndexes.x))] * (BoneWeights.x / sum);
		skinning += boneTransforms[int(round(BoneIndexes.y))] * (BoneWeights.y / sum);
		skinning += boneTransforms[int(round(BoneIndexes.z))] * (BoneWeights.z / sum);
		skinning += boneTransforms[int(round(BoneIndexes.w))] * (BoneWeights.w / sum);
	
		pos = (float4(VertexPos, 1.0) * skinning).xyz;
		normal = (float4(VertexNormal, 0.0) * skinning).xyz;
	}

	gl_Position = ModelViewProjection * float4(pos, 1.0);

	PSVertexPos = pos;
	PSVertexNormal = normal;
	PSVertexUV = VertexUV0;
	PSVertexColor = float3(MaterialID);
	PSMaterialID = MaterialID;
}

#endif


#ifdef pixelShader

mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv) {
	// get edge vectors of the pixel triangle
	vec3 dp1 = dFdx(p);
	vec3 dp2 = dFdy(p);
	vec2 duv1 = dFdx(uv);
	vec2 duv2 = dFdy(uv);

	// solve the linear system
	vec3 dp2perp = cross(dp2, N);
	vec3 dp1perp = cross(N, dp1);
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	// construct a scale-invariant frame 
	float invmax = 1.0 / sqrt(max(dot(T, T), dot(B, B)));
	return mat3(normalize(T * invmax), normalize(B * invmax), N);
}

vec3 perturb_normal(vec3 N, vec2 texcoord, vec3 map, vec3 V)
{
	mat3 TBN = cotangent_frame(N, -V, texcoord);
	return normalize(TBN * map);
}


in float3 PSVertexPos;
in float3 PSVertexNormal;
in float2 PSVertexUV;
in float3 PSVertexColor;
flat in int PSMaterialID;

out float4 FragColor;

void main()
{
	FragColor.a = 1.0;

	float2 UV = PSVertexUV;

	// Global stuff
	float3 worldNormal = (WorldToModel * float4(PSVertexNormal, 0.0f)).xyz;
	float3 worldPos = (WorldToModel * float4(PSVertexPos, 1.0)).xyz;

	worldPos = float3(worldPos.x, -worldPos.z, worldPos.y);
	worldNormal = float3(worldNormal.x, -worldNormal.z, worldNormal.y);
	worldNormal = normalize(worldNormal);

	float3 cameraVector = normalize(worldPos - CameraPosition);
	FragColor.rgb = frac(worldPos);

	// Sample and  pick texture
	float3 M1 = texture(texM1Extra, UV).rgb;
	float3 M2 = texture(texM2Extra, UV).rgb;
	float3 M3 = texture(texM3Extra, UV).rgb;
	if (PSMaterialID == 1)
	{
		M1 = texture(texM1, UV).rgb;
		M2 = texture(texM2, UV).rgb;
		M3 = texture(texM3, UV).rgb;
	}
	
	// Tangent-Space Normal mapping
	M2.rg = M2.rg * 2.0 - 1.0;
	M2.g = -M2.g;
	float3 normal = float3(M2.rg, sqrt(saturate(1.0f - dot(M2.rg, M2.rg))));
	worldNormal = perturb_normal(worldNormal, UV, normalize(normal), -(cameraVector));
	
	// Lighting
	float NdotL = dot(worldNormal, float3(0, 0, 1));
	float halflambert = NdotL * 0.5 + 0.5;
	
	float3 lighting = float3(saturate(NdotL)) + float3(0.4, 0.3, 0.2);
	
	FragColor.rgb = M1 * lighting;
	
	if (M3.r < 0.5)
		discard;


	// Ensure it never goes below 0, headset colors are abs'd?
	FragColor.rgb = max(FragColor.rgb, float3(0, 0, 0));

}
#endif
