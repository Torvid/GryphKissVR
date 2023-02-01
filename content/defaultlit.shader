
#include "macros.shaderinc"

#define ShaderName defaultlit

#define Parameters(X) \
	X(float3, Color) \
	X(sampler2D, texM1) \
	X(sampler2D, texM2) \
	X(sampler2D, texCubemap) \
	X(sampler2D, texLightmap) \
	X(float3, lightmapMin) \
	X(float3, lightmapMax) \
	X(float3, lightmapResolution) \
	X(float, radiosityProbeScale) \

#define addBoneTransforms 1

#include "shaderMacros.cpp"
#include "shaderMacros.shaderinc"

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

	float3 cameraPos = CameraPosition;
	//float3 cameraPos = float3(3.0, 3.0, 3.0);
	float dist = distance(worldPos, cameraPos);
	
	//dist = dist / (1.0 + abs(dist));
	////float3 angle = AngleDecode(float2());
	//gl_Position = float4(OctEncode(normalize(worldPos - cameraPos)), dist, 1);// OctEncode(worldPos);// float4(OctEncode(worldPos), 0, 0);

	PSVertexPos = worldPos;
	PSVertexNormal = normalize(worldNormal);
	PSVertexTangent = normalize(worldTangent);
	PSVertexBitangent = normalize(cross(VertexTangent, worldNormal));
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

float2 PosToUV(float3 pos)
{
	float2 LightmapUV = float2(pos.x + lightmapResolution.x * pos.y, pos.z);
	LightmapUV /= float2(lightmapResolution.x * lightmapResolution.y, lightmapResolution.z);

	return LightmapUV;
}

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

	float3 bakedAmbientSampleValue = floor((worldPos - lightmapMin) / radiosityProbeScale);
	float3 bakedAmbientSampleBlend = frac((worldPos - lightmapMin) / radiosityProbeScale);
	bakedAmbientSampleValue += float3(1.5, 1.5, 1.5) + floor(float3(6.7, -1.9, -0.3));// floor(CameraPosition);
	//float3 bakedAmbientSampleUV = (worldPos - lightmapMin) / abs(lightmapMin - lightmapMax);

	float2 LightmapUV000 = PosToUV(bakedAmbientSampleValue + float3(0, 0, 0));
	float2 LightmapUV100 = PosToUV(bakedAmbientSampleValue + float3(1, 0, 0));
	float2 LightmapUV010 = PosToUV(bakedAmbientSampleValue + float3(0, 1, 0));
	float2 LightmapUV110 = PosToUV(bakedAmbientSampleValue + float3(1, 1, 0));
	float2 LightmapUV001 = PosToUV(bakedAmbientSampleValue + float3(0, 0, 1));
	float2 LightmapUV101 = PosToUV(bakedAmbientSampleValue + float3(1, 0, 1));
	float2 LightmapUV011 = PosToUV(bakedAmbientSampleValue + float3(0, 1, 1));
	float2 LightmapUV111 = PosToUV(bakedAmbientSampleValue + float3(1, 1, 1));

	float3 lightmapSample000 = Sample(texLightmap, LightmapUV000).rgb;
	float3 lightmapSample100 = Sample(texLightmap, LightmapUV100).rgb;
	float3 lightmapSample010 = Sample(texLightmap, LightmapUV010).rgb;
	float3 lightmapSample110 = Sample(texLightmap, LightmapUV110).rgb;
	float3 lightmapSample001 = Sample(texLightmap, LightmapUV001).rgb;
	float3 lightmapSample101 = Sample(texLightmap, LightmapUV101).rgb;
	float3 lightmapSample011 = Sample(texLightmap, LightmapUV011).rgb;
	float3 lightmapSample111 = Sample(texLightmap, LightmapUV111).rgb;
	
	float3 lightmap =
		lerp(lerp(lerp(lightmapSample000, lightmapSample100, bakedAmbientSampleBlend.x),
				  lerp(lightmapSample001, lightmapSample101, bakedAmbientSampleBlend.x), bakedAmbientSampleBlend.z),
			 lerp(lerp(lightmapSample010, lightmapSample110, bakedAmbientSampleBlend.x),
			 lerp(	   lightmapSample011, lightmapSample111, bakedAmbientSampleBlend.x), bakedAmbientSampleBlend.z), bakedAmbientSampleBlend.y);
	lightmap *= 3.0;
	//lightmap *= 0.5;
	//lightmap = saturate(float3(LightmapUV, 0));

	//int3 bakedAmbientSamplePos = int3(floor(bakedAmbientSamplePos01 * lightmapResolution));
	//float2 bakedAmbientUV = 
	//float3 bakedAmbientLight = 

	float3 cameraVector = (PSVertexPos - CameraPosition);
	float3 reflectVector = reflect(cameraVector, PSVertexNormal);
	//reflectVector.x = -reflectVector.x;
	reflectVector.yz = -reflectVector.yz;
	float2 reflectUV = OctEncode((reflectVector));
	//float4 cubemap = Sample(texCubemap, reflectUV);
	float4 cubemap = SampleGrad(texCubemap, reflectUV, ddxy(UV*10.0f));

	float3 lighting = float3(saturate(NdotL)) + float3(0.4, 0.3, 0.2) + cubemap.rgb * 2.0f;

	FragColor.rgb = M1.rgb * lightmap;// lighting;
	//FragColor.rgb = lightmap;

	//FragColor.rgb = PSVertexNormal;
	//FragColor.rgb = (CameraPosition - PSVertexPos) * 10.0;
	if (M2.a < 0.5)
		discard;
	
	// Ensure it never goes below 0, headset colors are abs'd?
	FragColor.rgb = max(FragColor.rgb, float3(0, 0, 0));
}
#endif
