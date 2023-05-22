#include "../haven.cpp"

#if structs
struct ReflectionProbe
{
    EntityContents;
    Texture* cubeTexture[6];
    Texture* octTexture0;
    Texture* octTexture1;
    Texture* octTexture2;
    Texture* octTexture3;
    Texture* octTexture4;
    bool rendered;
};

#else

ReflectionProbe* ReflectionProbeInstantiate(Transform transform)
{
    ReflectionProbe* self = Instantiate(ReflectionProbe);

    for (int i = 0; i < 6; i++)
    {
        self->cubeTexture[i] = Rendering::CreateTextureTarget(512, 512, true);
    }
    self->octTexture0 = Rendering::CreateTextureTarget(512, 512, true);
    self->octTexture1 = Rendering::CreateTextureTarget(256, 256, true);
    self->octTexture2 = Rendering::CreateTextureTarget(128, 128, true);
    self->octTexture3 = Rendering::CreateTextureTarget(64, 64, true);
    self->octTexture4 = Rendering::CreateTextureTarget(32, 32, true);

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
    if (haven->editor)
    {
        //Drawing::DrawText("hello world");
        Drawing::DrawFontCameraFacing("\n\n\n\n\nReflection Probe", self->transform.position, 1.0f, 999, HAlign_center, VAlign_center);

        CreateMaterialLocal(command, assets->reflectionProbeShader, reflectionProbeShader);
        command->ColorTexture = self->octTexture0;
        command->Color = float3(1.0f, 1.0f, 1.0f);
        command->mesh = assets->sphereMesh;
        command->transform = self->transform;
        command->transform.scale = float3(0.25, 0.25, 0.25);
        Rendering::DrawMesh(command);
        Transform box = self->transform;
        box.scale *= 0.5;
        Drawing::DrawBox(box);
    }

    if (!self->rendered)
    {
        //float3 startPos = haven->spectatorCamera.position + float3(-0.5, 2, -0.5);
        //Rendering::SetCubemap(0);
        Rendering::RenderCubemap(self->transform.position, self->cubeTexture);
        Rendering::PackCubemap(self->octTexture0, self->cubeTexture);
        Rendering::Downsize4x(self->octTexture0, self->octTexture1, 20);
        Rendering::Downsize4x(self->octTexture1, self->octTexture2, 50);
        Rendering::Downsize4x(self->octTexture2, self->octTexture3, 400);
        Rendering::Downsize4x(self->octTexture3, self->octTexture4, 0);
        //Rendering::SetCubemap(self);
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
