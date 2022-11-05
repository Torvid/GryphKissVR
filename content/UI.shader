
#include "macros.shader"

#define ShaderName UI

#define Parameters(X) \
	X(float3, VRCameraPosition) \
	X(float3, VRCameraForward) \
	X(float3, VRCameraUp) \
	X(float3, VRCameraRight) \
	X(float3, SpectatorCameraPosition) \
	X(float3, SpectatorCameraForward) \
	X(float3, SpectatorCameraUp) \
	X(float3, SpectatorCameraRight) \
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
in float3 VertexTangent;
in float2 VertexUV0;
in float2 VertexUV1;
in int MaterialID;

out float3 PSVertexPos;
out float3 PSVertexNormal;
out float3 PSVertexTangent;
out float2 PSVertexUV0;
out float2 PSVertexUV1;
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
	float4x4 headset = float4x4(
		float4(VRCameraRight, 0),
		float4(VRCameraForward, 0),
		float4(VRCameraUp, 0),
		float4(VRCameraPosition, 1));

	float4x4 spectator = float4x4(
		float4(SpectatorCameraRight, 0),
		float4(-SpectatorCameraForward, 0),
		float4(-SpectatorCameraUp, 0),
		float4(VertexTangent, 1));

	float4x4 headset2 = float4x4(
		float4(VRCameraRight, 0),
		float4(-VRCameraForward, 0),
		float4(-VRCameraUp, 0),
		float4(VertexTangent, 1));

	bool Is3DText = VertexUV1.y > 0.5 && VertexUV1.y < 1.5;
	bool Is3DMesh = VertexUV1.y > 1.5 && VertexUV1.y < 2.5;
	

	if (IsSpectatorView > 0.5)
	{
		float2 pos = pixelPosToScreenPos(VertexPos.xy, Resolution);
		gl_Position = float4(pos, 0.00001f, 1.0);
		if (Is3DText)
		{
			gl_Position = ModelViewProjection * spectator * float4(VertexPos, 1.0f);
		}
		else if (Is3DMesh)
		{
			gl_Position = ModelViewProjection * float4(VertexPos, 1.0f);
		}
	}
	else
	{
		float2 pos = pixelPosToScreenPos(VertexPos.xy, float2(1080, 1080) * 0.5);
		gl_Position = ModelViewProjection * headset * float4(pos.x, 0.0f, pos.y, 1.0);
		if (Is3DText)
		{
			gl_Position = ModelViewProjection * headset2 * float4(VertexPos, 1.0f);
		}
		else if (Is3DMesh)
		{
			gl_Position = ModelViewProjection * float4(VertexPos, 1.0f);
		}
	}

	PSVertexPos		= VertexPos;
	PSVertexNormal	= VertexNormal;
	PSVertexTangent	= VertexTangent;
	PSVertexUV0		= float2(VertexUV0.x, 1.0 - VertexUV0.y);
	PSVertexUV1		= VertexUV1;
	PSMaterialID	= MaterialID;
}

#endif

#ifdef pixelShader

in float3 PSVertexPos;
in float3 PSVertexNormal;
in float3 PSVertexTangent;
in float2 PSVertexUV0;
in float2 PSVertexUV1;
flat in int PSMaterialID;

out float4 FragColor;

void main()
{
	float4 M1 = Sample(FontTexture, PSVertexUV0);

	FragColor.rgb = PSVertexNormal; // M1.rgb * 
	FragColor.a = M1.a * PSVertexUV1.x;
}
#endif
