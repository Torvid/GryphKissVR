
#include "macros.shaderinc"

#define ShaderName defaultlit

#define Parameters(X) \
	X(float3, Color, float3(1, 1, 1)) \
	X(float3, OverlayColor, float3(0, 0, 0)) \
	X(sampler2D, texM1, assets->baseM1) \
	X(sampler2D, texM2, assets->baseM2) \
	X(sampler2D, texM3, assets->baseM3) \
	X(sampler2D, texCubemap0) \
	X(sampler2D, texCubemap1) \
	X(sampler2D, texCubemap2) \
	X(sampler2D, texCubemap3) \
	X(sampler2D, texCubemap4) \
	X(float3, cubemapPosition) \
	X(float3, cubemapSize) \
	X(sampler2D, texLightmap) \
	X(float3, lightmapMin) \
	X(float3, lightmapMax) \
	X(float3, lightmapResolution) \
	X(float, radiosityProbeScale) \
	X(float, lightmapStrength) \
	X(float, metallicOffset) \
	X(float, roughnessOffset) \

#define addBoneTransforms 1

#include "shaderMacros.cpp"
#include "shaderMacros.shaderinc"

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
out float2 PSVertexUV0;
out float2 PSVertexUV1;
out int PSMaterialID;

out float3 PSLightmap;

out float3 PSL00;
out float3 PSL11;
out float3 PSL10;
out float3 PSL1_1;
out float3 PSL21;
out float3 PSL2_1;
out float3 PSL2_2;
out float3 PSL20;
out float3 PSL22;

float2 PosToUV(float3 pos)
{
	float2 LightmapUV = float2(pos.x + lightmapResolution.x * pos.y, pos.z);
	LightmapUV /= float2(lightmapResolution.x * lightmapResolution.y, lightmapResolution.z);

	return LightmapUV;
}

struct SHData
{
	float3 L00;
	float3 L11;
	float3 L10;
	float3 L1_1;
	float3 L21;
	float3 L2_1;
	float3 L2_2;
	float3 L20;
	float3 L22;
};

SHData SHData_lerp(SHData a, SHData b, float t)
{
	SHData result;
	result.L00  = lerp(a.L00   , b.L00   , t);
	result.L11  = lerp(a.L11   , b.L11   , t);
	result.L10  = lerp(a.L10   , b.L10   , t);
	result.L1_1 = lerp(a.L1_1  , b.L1_1  , t);
	result.L21  = lerp(a.L21   , b.L21   , t);
	result.L2_1 = lerp(a.L2_1  , b.L2_1  , t);
	result.L2_2 = lerp(a.L2_2  , b.L2_2  , t);
	result.L20  = lerp(a.L20   , b.L20   , t);
	result.L22  = lerp(a.L22   , b.L22   , t);
	return result;
}

SHData ExtractSHData(float3 voxelPos)
{
	SHData result;

	float2 textureResolution = float2(lightmapResolution.x * lightmapResolution.y, lightmapResolution.z * 9.0);

	float xValue = voxelPos.x + lightmapResolution.x * voxelPos.y;
	float yValue = voxelPos.z * 9.0;
	float2 LightmapUV = float2(xValue, yValue);

	result.L00 = SampleLoad(texLightmap, LightmapUV + float2(0, 0), textureResolution).rgb;
	result.L11 = SampleLoad(texLightmap, LightmapUV + float2(0, 1), textureResolution).rgb;
	result.L10 = SampleLoad(texLightmap, LightmapUV + float2(0, 2), textureResolution).rgb;
	result.L1_1 = SampleLoad(texLightmap, LightmapUV + float2(0, 3), textureResolution).rgb;
	result.L21 = SampleLoad(texLightmap, LightmapUV + float2(0, 4), textureResolution).rgb;
	result.L2_1 = SampleLoad(texLightmap, LightmapUV + float2(0, 5), textureResolution).rgb;
	result.L2_2 = SampleLoad(texLightmap, LightmapUV + float2(0, 6), textureResolution).rgb;
	result.L20 = SampleLoad(texLightmap, LightmapUV + float2(0, 7), textureResolution).rgb;
	result.L22 = SampleLoad(texLightmap, LightmapUV + float2(0, 8), textureResolution).rgb;
	return result;
}

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

	float3 bakedAmbientSampleValue = floor((worldPos - lightmapMin) / radiosityProbeScale);
	float3 bakedAmbientSampleBlend = frac((worldPos - lightmapMin) / radiosityProbeScale);
	bakedAmbientSampleBlend = smooth_step(bakedAmbientSampleBlend); // tri-cubic blend

	SHData SHData000 = ExtractSHData(bakedAmbientSampleValue + float3(0, 0, 0));
	SHData SHData100 = ExtractSHData(bakedAmbientSampleValue + float3(1, 0, 0));
	SHData SHData010 = ExtractSHData(bakedAmbientSampleValue + float3(0, 1, 0));
	SHData SHData110 = ExtractSHData(bakedAmbientSampleValue + float3(1, 1, 0));
	SHData SHData001 = ExtractSHData(bakedAmbientSampleValue + float3(0, 0, 1));
	SHData SHData101 = ExtractSHData(bakedAmbientSampleValue + float3(1, 0, 1));
	SHData SHData011 = ExtractSHData(bakedAmbientSampleValue + float3(0, 1, 1));
	SHData SHData111 = ExtractSHData(bakedAmbientSampleValue + float3(1, 1, 1));

	SHData lightmap =
		SHData_lerp(SHData_lerp(SHData_lerp(SHData000, SHData100, bakedAmbientSampleBlend.x),
		SHData_lerp(SHData001, SHData101, bakedAmbientSampleBlend.x), bakedAmbientSampleBlend.z),
		SHData_lerp(SHData_lerp(SHData010, SHData110, bakedAmbientSampleBlend.x),
		SHData_lerp(SHData011, SHData111, bakedAmbientSampleBlend.x), bakedAmbientSampleBlend.z), bakedAmbientSampleBlend.y);
	
	float A0 = 3.141593;
	float A1 = 2.094395;
	float A2 = 0.785398;

	PSL00  = A0 * 0.282095 * lightmap.L00  ;
	PSL11  = A1 * 0.488603 * lightmap.L11  ;
	PSL10  = A1 * 0.488603 * lightmap.L10  ;
	PSL1_1 = A1 * 0.488603 * lightmap.L1_1 ;
	PSL21  = A2 * 1.092548 * lightmap.L21  ;
	PSL2_1 = A2 * 1.092548 * lightmap.L2_1 ;
	PSL2_2 = A2 * 1.092548 * lightmap.L2_2 ;
	PSL20  = A2 * 0.946176 * lightmap.L20  ;
	PSL22  = A2 * 0.546274 * lightmap.L22  ;

	PSLightmap = float3(0, 0, 0);

	PSVertexPos = worldPos;
	PSVertexNormal = normalize(worldNormal);
	PSVertexTangent = normalize(worldTangent);
	PSVertexBitangent = normalize(cross(VertexTangent, worldNormal));
	PSVertexUV0 = VertexUV0;
	PSVertexUV1 = VertexUV1;
	PSMaterialID = MaterialID;
}

#endif


#ifdef pixelShader
float3 raybox(float3 start, float3 direction, float3 center, float3 size)
{
	float3 tMin, tMax, t1, t2;
	float tNear, tFar;

	// Calculate the half-size of the box
	float3 halfSize = size;// *0.5;

	// Calculate the min and max points of the box
	float3 boxMin = center - halfSize;
	float3 boxMax = center + halfSize;

	// Calculate intersection times for each axis
	t1 = (boxMin - start) / direction;
	t2 = (boxMax - start) / direction;

	// Find the entry and exit points of the ray for each axis
	tMin = min(t1, t2);
	tMax = max(t1, t2);

	// Find the overall entry and exit points of the ray
	tNear = max(max(tMin.x, tMin.y), tMin.z);
	tFar = min(min(tMax.x, tMax.y), tMax.z);

	// If the ray intersects the box
	if (tNear < tFar && tFar > 0.0)
	{
		// Calculate the intersection point on the inside of the box
		float3 hit = start + direction * (tFar + float3(0.0001));

		// Calculate the vector pointing from the center to the ray hit
		float3 centerToHit = hit - center;

		//float3 normal = (hit - center) / halfSize;
		//normal = float3((abs(normal.x) > 0.5) ? sign(normal.x) : 0.0,
		//				(abs(normal.y) > 0.5) ? sign(normal.y) : 0.0,
		//				(abs(normal.z) > 0.5) ? sign(normal.z) : 0.0);

		return centerToHit;
	}

	// If there is no intersection, return 0
	return float3(0, 0, 0);
}
float3 SampleSphericalHarmonic(float3 voxelPos, float3 lightNormal)
{
	float A0 = 3.141593;
	float A1 = 2.094395;
	float A2 = 0.785398;

	float Y00 = 0.282095;
	float Y11 = 0.488603 * lightNormal.x;
	float Y10 = 0.488603 * lightNormal.z;
	float Y1_1 = 0.488603 * lightNormal.y;
	float Y21 = 1.092548 * lightNormal.x * lightNormal.z;
	float Y2_1 = 1.092548 * lightNormal.y * lightNormal.z;
	float Y2_2 = 1.092548 * lightNormal.y * lightNormal.x;
	float Y20 = 0.946176 * lightNormal.z * lightNormal.z - 0.315392;
	float Y22 = 0.546274 * (lightNormal.x * lightNormal.x - lightNormal.y * lightNormal.y);

	float2 textureResolution = float2(lightmapResolution.x * lightmapResolution.y, lightmapResolution.z * 9.0);

	float xValue = voxelPos.x + lightmapResolution.x * voxelPos.y;
	float yValue = voxelPos.z * 9.0;
	float2 LightmapUV = float2(xValue, yValue);

	float3 L00 = SampleLoad(texLightmap, LightmapUV + float2(0, 0), textureResolution).rgb;
	float3 L11 = SampleLoad(texLightmap, LightmapUV + float2(0, 1), textureResolution).rgb;
	float3 L10 = SampleLoad(texLightmap, LightmapUV + float2(0, 2), textureResolution).rgb;
	float3 L1_1 = SampleLoad(texLightmap, LightmapUV + float2(0, 3), textureResolution).rgb;
	float3 L21 = SampleLoad(texLightmap, LightmapUV + float2(0, 4), textureResolution).rgb;
	float3 L2_1 = SampleLoad(texLightmap, LightmapUV + float2(0, 5), textureResolution).rgb;
	float3 L2_2 = SampleLoad(texLightmap, LightmapUV + float2(0, 6), textureResolution).rgb;
	float3 L20 = SampleLoad(texLightmap, LightmapUV + float2(0, 7), textureResolution).rgb;
	float3 L22 = SampleLoad(texLightmap, LightmapUV + float2(0, 8), textureResolution).rgb;

	//return L00 * 0.8;

	return (A0 * Y00 * L00)
		+ (A1 * Y1_1 * L1_1) + (A1 * Y10 * L10) + (A1 * Y11 * L11)
		+ (A2 * Y2_2 * L2_2) + (A2 * Y2_1 * L2_1) + (A2 * Y20 * L20) + (A2 * Y21 * L21) + (A2 * Y22 * L22);

}

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
in float2 PSVertexUV0;
in float2 PSVertexUV1;
flat in int PSMaterialID;
in float3 PSLightmap;
in float3 PSL00;
in float3 PSL11;
in float3 PSL10;
in float3 PSL1_1;
in float3 PSL21;
in float3 PSL2_1;
in float3 PSL2_2;
in float3 PSL20;
in float3 PSL22;

out float4 FragColor;


void main()
{
	float2 UV = PSVertexUV0;
	float3 worldPos = PSVertexPos;
	
	float4 M1 = Sample(texM1, UV);
	float4 M2 = Sample(texM2, UV);
	float4 M3 = Sample(texM3, UV);
	// M1 RGB - Color
	// M1 A   - Roughness
	// 
	// M2 RG  - Normal
	// M2 B   - AO
	// M2 A   - Metallic
	// 
	// M3 RGB - Emissive
	// M3 A   - Alpha


	// Tangent-Space Normal mapping
	M2.rg = M2.rg * 2.0 - 1.0;
	float3 worldNormal = normalize(PSVertexNormal * 1.0 + M2.r * PSVertexTangent + M2.g * PSVertexBitangent);

	float3 lightNormal = float3(worldNormal.x, -worldNormal.y, -worldNormal.z);


#define pixelLightmaps 1
#if pixelLightmaps

	float3 bakedAmbientSampleValue = floor((worldPos - lightmapMin) / radiosityProbeScale);
	float3 bakedAmbientSampleBlend = frac((worldPos - lightmapMin) / radiosityProbeScale);

	//bakedAmbientSampleBlend = bakedAmbientSampleBlend * bakedAmbientSampleBlend * (3.0f - 2.0f * bakedAmbientSampleBlend);

	float3 lightmapSample000 = SampleSphericalHarmonic(bakedAmbientSampleValue + float3(0, 0, 0), lightNormal);
	float3 lightmapSample100 = SampleSphericalHarmonic(bakedAmbientSampleValue + float3(1, 0, 0), lightNormal);
	float3 lightmapSample010 = SampleSphericalHarmonic(bakedAmbientSampleValue + float3(0, 1, 0), lightNormal);
	float3 lightmapSample110 = SampleSphericalHarmonic(bakedAmbientSampleValue + float3(1, 1, 0), lightNormal);
	float3 lightmapSample001 = SampleSphericalHarmonic(bakedAmbientSampleValue + float3(0, 0, 1), lightNormal);
	float3 lightmapSample101 = SampleSphericalHarmonic(bakedAmbientSampleValue + float3(1, 0, 1), lightNormal);
	float3 lightmapSample011 = SampleSphericalHarmonic(bakedAmbientSampleValue + float3(0, 1, 1), lightNormal);
	float3 lightmapSample111 = SampleSphericalHarmonic(bakedAmbientSampleValue + float3(1, 1, 1), lightNormal);

	float3 bakedLighting =
		lerp(lerp(lerp(lightmapSample000, lightmapSample100, bakedAmbientSampleBlend.x),
			lerp(lightmapSample001, lightmapSample101, bakedAmbientSampleBlend.x), bakedAmbientSampleBlend.z),
			lerp(lerp(lightmapSample010, lightmapSample110, bakedAmbientSampleBlend.x),
				lerp(lightmapSample011, lightmapSample111, bakedAmbientSampleBlend.x), bakedAmbientSampleBlend.z), bakedAmbientSampleBlend.y);

#else
	float Y11 = lightNormal.x;
	float Y10 = lightNormal.z;
	float Y1_1 = lightNormal.y;
	float Y21 = lightNormal.x * lightNormal.z;
	float Y2_1 = lightNormal.y * lightNormal.z;
	float Y2_2 = lightNormal.y * lightNormal.x;
	float Y20 = lightNormal.z * lightNormal.z - 0.315392;
	float Y22 = (lightNormal.x * lightNormal.x - lightNormal.y * lightNormal.y);

	// Skip the third layer on quest for speed reasons.
	float3 bakedLighting = (PSL00)
		+(Y1_1 * PSL1_1) + (Y10 * PSL10) + (Y11 * PSL11);
	+(Y2_2 * PSL2_2) + (Y2_1 * PSL2_1) + (Y20 * PSL20) + (Y21 * PSL21) + (Y22 * PSL22);

#endif
	



	//float3 worldNormal = deriveTangentBasis(PSVertexNormal, worldPos, UV, float3(M2.rg, 1));

	// Lighting
	//float NdotL = dot(worldNormal, float3(0, 1, 1));
	//float halflambert = NdotL * 0.5 + 0.5;

	float3 cameraVector = normalize(PSVertexPos - CameraPosition);
	float3 reflectVector = reflect(cameraVector, worldNormal);
	reflectVector = raybox(worldPos, reflectVector, cubemapPosition, cubemapSize);
	reflectVector.yz = -reflectVector.yz;
	float2 reflectUV = OctEncode(reflectVector);
	float4 cubemap0 = Sample(texCubemap0, reflectUV);
	float4 cubemap1 = Sample(texCubemap1, reflectUV);
	float4 cubemap2 = Sample(texCubemap2, reflectUV);
	float4 cubemap3 = Sample(texCubemap3, reflectUV);
	float4 cubemap4 = Sample(texCubemap4, reflectUV);

	float roughness = saturate(M1.a + roughnessOffset);// saturate(UV.x + roughnessOffset);

	//roughness = 1.0f - roughness;
	//roughness = roughness * roughness * roughness;
	//roughness = 1.0f - roughness;
	float rBlend = roughness * 4.0;
	float4 cubemap = 
		lerp(cubemap0, 
			lerp(cubemap1, 
				lerp(cubemap2, 
					lerp(cubemap3,
							cubemap4, 
					smooth_step(saturate(rBlend - 3.0))),
				smooth_step(saturate(rBlend - 2.0))),
			smooth_step(saturate(rBlend - 1.0))),
		smooth_step(saturate(rBlend)));


	//float3 lighting = float3(saturate(NdotL)) + float3(0.4, 0.3, 0.2) + cubemap.rgb * 2.0f;

	float metallic = saturate(M2.a + metallicOffset);
	// I added these pbr vibes:
	// 1. Metallics tint reflections, normals do not
	// 2. Non-Metals reflect more at steep angles
	// 3. The more roughness a surface has, the more we blur the reflection cubemap

	float3 baseColor = M1.rgb * Color;

	float fresnel = 1.0f - dot(-cameraVector, worldNormal);

	float3 metallicResult = cubemap.rgb * baseColor;
	float3 dielectricResult = baseColor * bakedLighting * lightmapStrength * 0.5 + cubemap.rgb * fresnel * M2.b;

	//float3 d = 1.0f - abs(PSVertexPos - CameraPosition);
	FragColor.rgb = lerp(dielectricResult, metallicResult, metallic);

	//FragColor.rgb = cubemap.rgb;

	// emmissive
	FragColor.rgb += M3.rgb * 10.0;
	
	// editor
	FragColor.rgb += OverlayColor;

	if (M3.a < 0.5)
		discard;
	
	// Ensure it never goes below 0, headset colors are abs'd?
	FragColor.rgb = max(FragColor.rgb, float3(0, 0, 0));
}
#endif
