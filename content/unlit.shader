
#include "macros.shaderinc"

#define ShaderName unlit

#define Parameters(X) \
	X(float3, Color)\
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
	PSVertexPos = VertexPos;
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

	FragColor.rgb = Sample(ColorTexture, PSVertexUV).rgb * Color;
	//FragColor.rgb = textureLod(ColorTexture, UV, 4.0).rgb * Color;
	//FragColor.rgb = textureGrad(ColorTexture, UV, float2(0.1, 0), float2(0, 0)).rgb * Color;
	FragColor.a = 1.0;
}
#endif
