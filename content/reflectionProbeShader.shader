
#include "macros.shaderinc"

#define ShaderName reflectionProbeShader

#define Parameters(X) \
	X(float3, Color, float3(1, 1, 1))\
	X(float3, OverlayColor, float3(0, 0, 0)) \
	X(sampler2D, ColorTexture)

#include "shaderMacros.cpp"
#include "shaderMacros.shaderinc"

#ifdef vertexShader

in float3 VertexPos;
in float3 VertexNormal;
in float2 VertexUV0;

out float3 PSVertexPos;
out float3 PSVertexNormal;
out float3 PSVertexTangent;
out float2 PSVertexUV;

void main()
{
	gl_Position = ModelViewProjection * float4(VertexPos, 1.0);

	float3 worldPos = (WorldToModel * float4(VertexPos.xyz, 1.0)).xyz;
	float3 worldNormal = (WorldToModel * float4(VertexNormal, 0.0f)).xyz;
	worldPos = float3(worldPos.x, -worldPos.z, worldPos.y);
	worldNormal = float3(worldNormal.x, -worldNormal.z, worldNormal.y);
	PSVertexPos = worldPos;
	PSVertexNormal = normalize(worldNormal);
}

#endif


#ifdef pixelShader

in float3 PSVertexPos;
in float3 PSVertexNormal;
in float3 PSVertexTangent;
in float2 PSVertexUV;

out float4 FragColor;

void main()
{
	float2 UV = PSVertexUV;
	float3 cameraVector = normalize(PSVertexPos - CameraPosition);
	float3 reflectVector = reflect(cameraVector, PSVertexNormal);
	reflectVector.yz = -reflectVector.yz;
	float2 reflectUV = OctEncode(reflectVector);
	//float4 cubemap = SampleGrad(texCubemap, reflectUV, ddxy(UV*10.0f));

	FragColor.rgb = Sample(ColorTexture, reflectUV).rgb* Color;
	FragColor.rgb += OverlayColor;
	FragColor.a = 1.0;
}
#endif
