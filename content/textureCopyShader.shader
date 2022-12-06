
#include "macros.shaderinc"


#define ShaderName textureCopyShader

#define Parameters(X) \
	X(float3, Color)\
	X(sampler2D, ColorTexture)

#include "shaderMacros.cpp"
#include "shaderMacros.shaderinc"

//uniform float3 Color;
//uniform sampler2D ColorTexture;

#ifdef vertexShader

in float3 VertexPos;
in float3 VertexNormal;
in float2 VertexUV0;

out float3 PSVertexPos;
out float3 PSVertexNormal;
out float2 PSVertexUV;

void main()
{
	gl_Position = float4(VertexPos.xy * 2.0 - 1.0, 0.0f, 1.0);

	PSVertexPos = VertexPos;
	PSVertexNormal = VertexNormal;
	PSVertexUV = float2(VertexUV0.x, VertexUV0.y);
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
	FragColor = Sample(ColorTexture, PSVertexUV);
	if (Color.x != 0.0f && Color.y != 0.0f && Color.z != 0.0f)
	{
		FragColor.rgb *= Color;
	}
}
#endif
