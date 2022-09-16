
#if Cpp

#else

precision highp float;
precision highp int;
precision lowp sampler2D;
precision lowp samplerCube;

#define float2 vec2
#define float3 vec3
#define float4 vec4

#define float2x2 mat2
#define float3x3 mat3
#define float4x4 mat4

#define int2 ivec2
#define int3 ivec3
#define int4 ivec4

#define frac fract
#define lerp mix
#define saturate(x) clamp(x, 0.0, 1.0)
#define rcp(x) 1.0f / x 
#define atan2 atan

#define ddx dFdx
#define ddy dFdy

float3 rsqrt(float3 a)
{
	return float3(pow(a.x, -0.5), pow(a.y, -0.5), pow(a.z, -0.5));
}
float rsqrt(float a)
{
	return pow(a, -0.5);
}
//float2 pow(float2 a, float b)
//{
//	return float2(pow(a.x, b), pow(a.y, b));
//}

uniform float4x4 ModelViewProjection;
uniform float4x4 WorldToModel;
uniform float2 Resolution;
uniform float IsSpectatorView;
uniform float IsSkinned;
uniform float3 CameraPosition;
uniform float3 CameraForward;

//layout(std140) uniform GlobalVariables
//{
//	float4 diffuse;
//	float4 ambient;
//};

float4 SamplePoint(sampler2D Texture, float2 UV)
{
	float2 size = float2(textureSize(Texture, 0)) - 1.0;
	float2 delta = (size) / (size + 1.0);
	float2 resizedUVs = ((floor((UV / delta) * size) + 0.5) * delta) / size;
	return texture(Texture, resizedUVs);
}
float4 Sample(sampler2D Texture, float2 UV)
{
	return texture(Texture, UV);
}
float4 SampleClamp(sampler2D Texture, float2 UV)
{
	float2 size = float2(textureSize(Texture, 0)) - 1.0;
	float2 delta = (size) / (size + 1.0);
	float2 resizedUVs = (((saturate(UV) * size) + 0.5) * delta) / size;
	return texture(Texture, resizedUVs);
}
float4 SamplePointClamp(sampler2D Texture, float2 UV)
{
	float2 size = float2(textureSize(Texture, 0)) - 1.0;
	float2 delta = (size) / (size + 1.0);
	float2 resizedUVs = ((floor(saturate((UV) / delta) * size) + 0.5) * delta) / size;
	return texture(Texture, resizedUVs);
}
#endif