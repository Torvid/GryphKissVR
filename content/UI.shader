
#include "macros.shader"

#define ShaderName UI

#define Parameters(X) \
	X(float2, Position)\
	X(float2, Size)\
	X(float2, SubUVPosition)\
	X(float2, SubUVSize)\
	X(float3, Color)\
	X(float, Opacity)\
	X(sampler2D, ColorTexture)

#include "shaderMacros.cpp"
#include "shaderMacros.shader"

#ifdef vertexShader

in float3 VertexPos;
in float3 VertexNormal;
in float2 VertexUV0;

out float3 PSVertexPos;
out float3 PSVertexNormal;
out float2 PSVertexUV;

float2 pixelPosToScreenPos(float2 localPos, float2 res)
{
	float2 pos = localPos / res;
	pos.x = ((pos.x * 2.0) - 1.0);
	pos.y = -((pos.y * 2.0) - 1.0);
	return pos;
}

void main()
{
	float2 res = Resolution;
	if (IsSpectatorView < 0.5)
	{
		res = float2(1080, 1080) * 0.5;
	}
	float2 p1 = pixelPosToScreenPos(Position, res);
	float2 p2 = pixelPosToScreenPos(Position + Size, res);
	float2 pos = lerp(p1, p2, VertexPos.xy);
	gl_Position = float4(pos, 0.00001f, 1.0);

	if (IsSpectatorView < 0.5)
	{
		gl_Position = ModelViewProjection * float4(pos.x, 0.0f, pos.y, 1.0);
	}

	PSVertexPos = VertexPos;
	PSVertexNormal = VertexNormal;
	PSVertexUV = float2(VertexUV0.x, 1.0 - VertexUV0.y);
}

#endif


#ifdef pixelShader

in float3 PSVertexPos;
in float3 PSVertexNormal;
in float2 PSVertexUV;

out float4 FragColor;


void main()
{
	float4 M1 = SamplePoint(ColorTexture, PSVertexUV);
	
	float Alpha = M1.a;

	if (Alpha < 0.5f)
		discard;

	FragColor.rgb = Color;
	FragColor.a = Opacity;
}
#endif
