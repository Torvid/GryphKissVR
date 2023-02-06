
#include "macros.shaderinc"

#define ShaderName skydomeShader

#define Parameters(X) \
	X(float3, Color) \
	X(sampler2D, ColorTexture)

#include "shaderMacros.cpp"
#include "shaderMacros.shaderinc"

#ifdef vertexShader

in float3 VertexPos;
in float3 VertexNormal;
in float2 VertexUV0;

out float3 PSVertexPos;
out float3 PSVertexNormal;
out float2 PSVertexUV;

void main()
{
	gl_Position = ModelViewProjection * float4(VertexPos, 1.0);
	float3 worldPos = (WorldToModel * float4(VertexPos, 1.0)).xyz;
	PSVertexPos = worldPos;
	PSVertexNormal = VertexNormal;
	PSVertexUV = VertexUV0;
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
	float2 UV = PSVertexUV * 8.0;
	float3 cameraVector = normalize(PSVertexPos - CameraPosition);
	float3 skyColor = float3(0.251, 0.298, 0.373);
	float3 sunColor = float3(0.373, 0.298, 0.251);

	float3 sunDirection = normalize(float3(-1, 0.8, 1));
	float sun = saturate(dot(cameraVector, sunDirection));// pow(saturate(dot(PSVertexPos, sunDirection)), 9999.0);
	sun = pow(sun, 250.0) * 1000.0;

	float3 result = skyColor;
	result += sun * sunColor;
	//if (cameraVector.z < 0.0)
	//	result = float3(0.0, 0.0, 0.0);

	FragColor.rgb = result;
	//FragColor.rgb = Sample(ColorTexture, PSVertexUV).rgb * Color;
	//FragColor.rgb = textureLod(ColorTexture, UV, 4.0).rgb * Color;
	//FragColor.rgb = textureGrad(ColorTexture, UV, float2(0.1, 0), float2(0, 0)).rgb * Color;
	FragColor.a = 1.0;
}
#endif
