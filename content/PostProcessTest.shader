
#include "macros.shader"

// Inputs

#define ShaderName PostProcessTest

#define Parameters(X) \
	X(float3, Color)\
	X(sampler2D, ColorTexture)\
	X(sampler2D, TexRipples)

#include "shaderMacros.cpp"
#include "shaderMacros.shader"

//uniform float3 Color;
//
//uniform sampler2D ColorTexture;
//uniform sampler2D TexRipples;

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
	//gl_Position = float4(VertexPos.xy, 0.0f, 1.0);

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
float3 pow3(float3 a, float b)
{
	return float3(pow(a.x, b), pow(a.y, b), pow(a.z, b));
}

void main()
{
	float PI = 3.14152128;

	float effectRadius = .2;
	float effectAngle = 1.0 * PI;

	float2 center = float2(0.5, 0.5);
	float2 uv = PSVertexUV - center;

	float len = length(uv);
	float angle = atan(uv.y, uv.x) + effectAngle * smoothstep(effectRadius, 0., len);
	float radius = length(uv);
	uv = float2(radius * cos(angle), radius * sin(angle)) + center;


	float4 Ripples = Sample(TexRipples, PSVertexUV);
	float4 M1 = Sample(ColorTexture, PSVertexUV);

	if (IsSpectatorView < 0.5)
	{
		M1.rgb = M1.rgb * M1.rgb;
	}

	FragColor.rgb =  (M1.rgb);
	FragColor.a = 1.0;
}
#endif
