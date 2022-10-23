
#include "macros.shader"

#define ShaderName UI

#define Parameters(X) \
	X(sampler2D, FontTexture) \
	X(sampler2D, Texture0) \
	X(sampler2D, Texture1) \
	X(sampler2D, Texture2) \
	X(sampler2D, Texture3) \
	X(sampler2D, Texture4) \
	X(sampler2D, Texture5) \
	X(sampler2D, Texture6) \
	X(sampler2D, Texture7) \


#include "shaderMacros.cpp"
#include "shaderMacros.shader"

#ifdef vertexShader

in float3 VertexPos;
in float3 VertexNormal;
in float2 VertexUV0;
in int MaterialID;

out float3 PSVertexPos;
out float3 PSVertexNormal;
out float2 PSVertexUV;
out int PSMaterialID;



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

	float2 pos = pixelPosToScreenPos(VertexPos.xy, res);

	gl_Position = float4(pos, 0.00001f, 1.0);
	//gl_Position = float4(pos, 1, 1.0);

	if (IsSpectatorView < 0.5)
	{
		gl_Position = ModelViewProjection * float4(pos.x, 0.0f, pos.y, 1.0);
	}


	PSVertexPos = VertexPos;
	PSVertexNormal = VertexNormal;
	PSVertexUV = float2(VertexUV0.x, 1.0 - VertexUV0.y);
	PSMaterialID = MaterialID;
}

#endif

#ifdef pixelShader

in float3 PSVertexPos;
in float3 PSVertexNormal;
in float3 PSVertexTangent;
in float2 PSVertexUV;
flat in int PSMaterialID;

out float4 FragColor;

void main()
{
	float4 M1 = Sample(FontTexture, PSVertexUV);

	FragColor.rgb = M1.rgb * PSVertexNormal;
	FragColor.a = M1.a * PSVertexPos.z;
}
#endif
