
#include "macros.shaderinc"

#define ShaderName reflectionProbeToSphericalHarmonic

#define Parameters(X) \
	X(sampler2D, cubeTexture0)\
	X(sampler2D, cubeTexture1)\
	X(sampler2D, cubeTexture2)\
	X(sampler2D, cubeTexture3)\
	X(sampler2D, cubeTexture4)\
	X(sampler2D, cubeTexture5)\

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
	float3 ray = OctDecode(PSVertexUV);
	float2 as = OctEncode(ray);
	ray = OctDecode(as);
	float3 res = RayCubemap(ray);
	float2 uv = res.xy;
	uv = saturate(uv);

	float4 sample0 = Sample(cubeTexture0, uv);
	float4 sample1 = Sample(cubeTexture1, uv);
	float4 sample2 = Sample(cubeTexture2, uv);
	float4 sample3 = Sample(cubeTexture3, uv);
	float4 sample4 = Sample(cubeTexture4, uv);
	float4 sample5 = Sample(cubeTexture5, uv);

	float4 resultSample;
	if (res.z == 0.0) resultSample = sample0;
	if (res.z == 1.0) resultSample = sample1;
	if (res.z == 2.0) resultSample = sample2;
	if (res.z == 3.0) resultSample = sample3;
	if (res.z == 4.0) resultSample = sample4;
	if (res.z == 5.0) resultSample = sample5;

	FragColor = resultSample;
}
#endif
