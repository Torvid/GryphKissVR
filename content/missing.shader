
#include "macros.shader"

#define ShaderName missing

#define Parameters(X) \
X(float3,Color)

#include "shaderMacros.cpp"
#include "shaderMacros.shader"

#ifdef vertexShader

in float3 VertexPos;
in float3 VertexNormal;
in float3 PSVertexTangent;
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
	FragColor.rgb = float3(1, 0, 1);
	FragColor.a = 1.0;
}
#endif
