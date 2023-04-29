#include "../haven.cpp"

#if structs
struct ReflectionProbe
{
    EntityContents;
    Texture* cubeTexture[6];
    Texture* octTexture;
    bool rendered;
};

#else
ReflectionProbe* ReflectionProbeInstantiate(Transform transform)
{
    ReflectionProbe* self = Instantiate(ReflectionProbe);

    for (int i = 0; i < 6; i++)
    {
        self->cubeTexture[i] = Rendering::CreateTextureTarget(128, 128, true);
    }
    self->octTexture = Rendering::CreateTextureTarget(512, 512, true);

    self->transform = transform;
    self->rendered = false;
    return self;
}

void DrawScreenTexture(Texture* texture, float2 size, float heightOffset, float3 Color = float3(1.0f, 1.0f, 1.0f))
{
    float scale = 0.04;
    Transform planeTransform = transform(haven->spectatorCamera.position
        + haven->spectatorCamera.forward * 0.1
        + haven->spectatorCamera.right * -0.08
        + haven->spectatorCamera.up * heightOffset,
        haven->spectatorCamera.right,
        haven->spectatorCamera.up,
        haven->spectatorCamera.forward,
        float3(size.x, size.y, 1) * scale);

    //float3(1, 0, 0),
    //float3(0, 0, 1),
    //float3(0, 1, 0),

    //planeTransform.position += haven->spectatorCamera.forward * 0.1;
    ////transform(haven->spectatorCamera.position + float3(0, 0.1, 0));
    //planeTransform.scale = float3(size.x, size.y, 1) * 0.1;
    ////planeTransform = LookRotation(planeTransform, haven->spectatorCamera.forward, haven->spectatorCamera.up);
    CreateMaterialLocal(command, assets->unlit, unlit);
    command->ColorTexture = texture;
    command->Color = Color;
    command->BackFaceCulling = true;
    Rendering::DrawMesh(command, assets->ui_quad, planeTransform, "Probe plane in the scene");
}
void ReflectionProbeUpdate(ReflectionProbe* self, int i)
{
    //Drawing::DrawText("hello world");
    Drawing::DrawFontCameraFacing("\n\n\n\n\nReflection Probe", self->transform.position, 1.0f, 999, HAlign_center, VAlign_center);

    CreateMaterialLocal(command, assets->reflectionProbeShader, reflectionProbeShader);
    command->ColorTexture = self->octTexture;
    command->Color = float3(1.0f, 1.0f, 1.0f);
    command->mesh = assets->sphereMesh;
    command->transform = self->transform;
    command->transform.scale = float3(0.25, 0.25, 0.25);
    Rendering::DrawMesh(command);

    if (!self->rendered)
    {
        //float3 startPos = haven->spectatorCamera.position + float3(-0.5, 2, -0.5);
        Rendering::RenderCubemap(self->transform.position, self->cubeTexture);
        Rendering::PackCubemap(self->octTexture, self->cubeTexture);
        self->rendered = true;
    }
    return;

    //DrawScreenTexture(self->octTexture);

    //DrawFont("Octahedral packing: ", transform(planeTransform.position), 1, 2.0f, HAlign_left, VAlign_up);

    //CreateMaterialLocal(waterPlane2, defaultlit);
    //waterPlane2->texM1 = assets->baseM1;
    //waterPlane2->texM2 = assets->baseM2;
    //waterPlane2->texCubemap = self->octTexture;
    //waterPlane2->Color = float3(1.0f, 1.0f, 1.0f);
    //planeTransform.scale = float3(0.25, 0.25, 0.25);
    //planeTransform.position = startPos + float3(-0.5, 0, -0.3);
    //DrawMesh(waterPlane2, assets->sphereMesh, planeTransform, "Probe plane in the scene");
}

#endif
