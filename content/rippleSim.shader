
#include "macros.shader"


#define ShaderName rippleSim

#define Parameters(X) \
	X(float2, mousePos)\
	X(sampler2D, TexPrevious)\


#include "shaderMacros.cpp"
#include "shaderMacros.shader"

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
in float2 PSVertexUV;

out float4 FragColor;

void main()
{
	float offset = 1.0f / 128.0f;

	float2 pos = float2(mousePos.x, 1.0 - mousePos.y);

	float force = saturate((0.05 - length(PSVertexUV - pos))*5.0);

	float4 center	= SampleClamp(TexPrevious, PSVertexUV + float2(0, 0));
	float4 left		= SampleClamp(TexPrevious, PSVertexUV + float2(offset, 0));
	float4 right	= SampleClamp(TexPrevious, PSVertexUV + float2(-offset, 0));
	float4 up		= SampleClamp(TexPrevious, PSVertexUV + float2(0, offset));
	float4 down		= SampleClamp(TexPrevious, PSVertexUV + float2(0, -offset));

	float targetHeight = (left.x + right.x + up.x + down.x) * 0.25f;
	float currentHeight = center.x;
	float currentVelocity = center.y;

	float delta = (targetHeight - currentHeight);

	float newVelocity = currentVelocity + delta;

	float newPosition = currentHeight + newVelocity + force;

	FragColor = float4(newPosition, newVelocity, 0, 0);
}
#endif
