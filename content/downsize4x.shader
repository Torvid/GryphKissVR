
#include "macros.shaderinc"

#define ShaderName downsize4x

#define Parameters(X) \
	X(sampler2D, colorTexture) \
	X(float, resolution)

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
	int stepCount = 2;
	int width = stepCount * 2 + 1;
	float pixelSize = 1.0 / resolution;
	pixelSize *= 1.0; // fudge

	float4 result = float4(0,0,0,0);
	float count = 0.0;
	for (int x = -stepCount; x < (stepCount+1); x++)
	{
		for (int y = -stepCount; y < (stepCount + 1); y++)
		{
			float2 uv = PSVertexUV + float2(x, y) * pixelSize;
			result += Sample(colorTexture, uv);
			count += 1.0;
		}
	}
	FragColor = result / count;

}
#endif
