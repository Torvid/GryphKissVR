
#include "macros.shaderinc"

#define ShaderName reflectionProbeCubemapToOct

float3 ArrayOfDirections[300];
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
	PSVertexUV = VertexUV0;
}
#endif

#ifdef pixelShader

float RadicalInverse_VdC(uint bits) {
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10;
}

float2 Hammersley(uint i, uint N) {
	return float2(float(i) / float(N), RadicalInverse_VdC(i));
}

float3 UniformSampleSphere(float2 u) {
	float z = 1.0 - 2.0 * u.x;
	float r = sqrt(max(0.0, 1.0 - z * z));
	float phi = 2.0 * 3.14159265359 * u.y;
	return float3(r * cos(phi), r * sin(phi), z);
}
const uint sampleCount = 4096u;

float3 RandomUnitVector(uint seed) {
	//uint N = 2048u; // Adjust this value to control the density of the distribution
	float2 u = Hammersley(uint(seed), sampleCount);
	return UniformSampleSphere(u);
}

in float2 PSVertexUV;
out float4 FragColor;
void main()
{
	float3 L00  = float3(0.0);
	float3 L11  = float3(0.0);
	float3 L10  = float3(0.0);
	float3 L1_1 = float3(0.0);
	float3 L21  = float3(0.0);
	float3 L2_1 = float3(0.0);
	float3 L2_2 = float3(0.0);
	float3 L20  = float3(0.0);
	float3 L22  = float3(0.0);

	for (uint i = 0u; i < sampleCount; i++)
	{
		float3 normal = RandomUnitVector(i);
		float3 res = RayCubemap(normal);

		float4 sample0 = Sample(cubeTexture0, res.xy);
		float4 sample1 = Sample(cubeTexture1, res.xy);
		float4 sample2 = Sample(cubeTexture2, res.xy);
		float4 sample3 = Sample(cubeTexture3, res.xy);
		float4 sample4 = Sample(cubeTexture4, res.xy);
		float4 sample5 = Sample(cubeTexture5, res.xy);

		float3 L;
		if (res.z == 0.0) L = sample0.rgb;
		if (res.z == 1.0) L = sample1.rgb;
		if (res.z == 2.0) L = sample2.rgb;
		if (res.z == 3.0) L = sample3.rgb;
		if (res.z == 4.0) L = sample4.rgb;
		if (res.z == 5.0) L = sample5.rgb;

		float Y00  = 0.282095;
		float Y11  = 0.488603 *  normal.x;
		float Y10  = 0.488603 *  normal.z;
		float Y1_1 = 0.488603 *  normal.y;
		float Y21  = 1.092548 *  normal.x * normal.z;
		float Y2_1 = 1.092548 *  normal.y * normal.z;
		float Y2_2 = 1.092548 *  normal.y * normal.x;
		float Y20  = 0.946176 *  normal.z * normal.z - 0.3120485392;
		float Y22  = 0.546274 * (normal.x * normal.x - normal.y * normal.y);

		L00  += L * Y00;
		L11  += L * Y11;
		L10  += L * Y10;
		L1_1 += L * Y1_1;
		L21  += L * Y21;
		L2_1 += L * Y2_1;
		L2_2 += L * Y2_2;
		L20  += L * Y20;
		L22  += L * Y22;
	}

	//L00	 /= 300.0;
	//L11	 /= 300.0;
	//L10	 /= 300.0;
	//L1_1 /= 300.0;
	//L21	 /= 300.0;
	//L2_1 /= 300.0;
	//L2_2 /= 300.0;
	//L20	 /= 300.0;
	//L22	 /= 300.0;

	float height = PSVertexUV.y * 9.0;
	float3 result = float3(0.0);

	if (height < 1.0)
		result = L00;
	else if (height < 2.0)
		result = L11;
	else if (height < 3.0)
		result = L10;
	else if (height < 4.0)
		result = L1_1;
	else if (height < 5.0)
		result = L21;
	else if (height < 6.0)
		result = L2_1;
	else if (height < 7.0)
		result = L2_2;
	else if (height < 8.0)
		result = L20;
	else if (height < 9.0)
		result = L22;

	result /= float(sampleCount);
	FragColor = float4(result * 30.0, 1.0);
}
#endif
