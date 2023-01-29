#include "macros.shaderinc"

#define ShaderName compose

#define Parameters(X) \
	X(float2, Min) \
	X(float2, Max) \
	X(sampler2D, texA) \
	X(sampler2D, texB)\
	X(float, composeMode)\


#include "shaderMacros.cpp"
#include "shaderMacros.shaderinc"


#ifdef vertexShader
in float3 VertexPos;
in float2 VertexUV0;
out float2 PSVertexUV;
void main()
{
	gl_Position = float4(VertexPos.xy * 2.0 - 1.0, 0.0f, 1.0);
	PSVertexUV = float2(VertexUV0.x, VertexUV0.y);
}
#endif


#ifdef pixelShader
in float2 PSVertexUV;
out float4 FragColor;
void main()
{
	float2 UV = (PSVertexUV - Min) / (Max - Min);

	//FragColor = float4(0, 1, 0, 1);
	//if (UV.x > 0.00001 && UV.x < 1.0 && UV.y > 0.00001 && UV.y < 1.0)
	//	FragColor = float4(1, 0, 0, 1);
	float4 a = Sample(texA, PSVertexUV);
	float4 b = Sample(texB, UV);
	int compMode = int(composeMode);

	FragColor = a;
	b = b * 10.0;
	if (UV.x > 0.00001 && UV.x < 1.0 && UV.y > 0.00001 && UV.y < 1.0)
	{
		if (compMode == 0)
			FragColor = a + b;
		else if (compMode == 1)
			FragColor = a - b;
		else if (compMode == 2)
			FragColor = a * b;
		else if (compMode == 3)
			FragColor = a / b;
		else if (compMode == 4)
			FragColor = max(a, b);
		else if (compMode == 5)
			FragColor = min(a, b);
		else if (compMode == 6)
			FragColor = float4(
				a.x == 0.0 ? b.x : a.x,
				a.y == 0.0 ? b.y : a.y,
				a.z == 0.0 ? b.z : a.z,
				a.w == 0.0 ? b.w : a.w);
	}
}
#endif
