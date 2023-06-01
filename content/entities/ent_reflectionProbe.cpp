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
    float3 Color;
};

#else
void ClearCubemaps(bool clear = false)
{
    for (int i = 0; i < ArrayCount(haven->entities); i++)
    {
        if (haven->entities[i]->type != EntityType_StaticMesh)
            continue;
        StaticMesh* mesh = (StaticMesh*)haven->entities[i];

        mesh->material.texCubemap0 = assets->black;
        mesh->material.texCubemap1 = assets->black;
        mesh->material.texCubemap2 = assets->black;
        mesh->material.texCubemap3 = assets->black;
        mesh->material.texCubemap4 = assets->black;
    }
}

void SetCubemaps()
{
    for (int i = 0; i < ArrayCount(haven->entities); i++)
    {
        if (haven->entities[i]->type != EntityType_StaticMesh)
            continue;

        StaticMesh* mesh = (StaticMesh*)haven->entities[i];
        int probeIndex = 0;

        float3 center = StaticMeshGetLocalBoundsTransform(mesh).position;

        ReflectionProbe* probe = (ReflectionProbe*)GetClosestEntityByType(center, EntityType_ReflectionProbe, &probeIndex);
        
        Transform meshAABB = StaticMeshGetAABB(mesh);
        Transform expandedAABB = CombineAABBs(meshAABB, probe->transform);

        if (!probe)
            continue;

        mesh->material.texCubemap0 = probe->octTexture0;
        mesh->material.texCubemap1 = probe->octTexture1;
        mesh->material.texCubemap2 = probe->octTexture2;
        mesh->material.texCubemap3 = probe->octTexture3;
        mesh->material.texCubemap4 = probe->octTexture4;
        mesh->material.cubemapPosition = expandedAABB.position;
        mesh->material.cubemapSize = expandedAABB.scale + float3(0.01, 0.01, 0.01);
    }

    for (int i = 0; i < ArrayCount(haven->sceneMaterials); i++)
    {
        ReflectionProbe* probe = (ReflectionProbe*)GetClosestEntityByType(float3(0, 0, 0), EntityType_ReflectionProbe);
    
        haven->sceneMaterials[i]->texCubemap0 = probe->octTexture0;
        haven->sceneMaterials[i]->texCubemap1 = probe->octTexture1;
        haven->sceneMaterials[i]->texCubemap2 = probe->octTexture2;
        haven->sceneMaterials[i]->texCubemap3 = probe->octTexture3;
        haven->sceneMaterials[i]->texCubemap4 = probe->octTexture4;
        haven->sceneMaterials[i]->cubemapPosition = probe->transform.position;
        haven->sceneMaterials[i]->cubemapSize = probe->transform.scale + float3(0.01, 0.01, 0.01);
    }
}

ReflectionProbe* ReflectionProbeInstantiate(Transform transform)
{
    ReflectionProbe* self = Instantiate(ReflectionProbe);
    self->Color = float3(1.0f, 1.0f, 1.0f);
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
        Debug::DrawFontCameraFacing("\n\n\n\n\nReflection Probe", self->transform.position, 1.0f, 999, HAlign_center, VAlign_center);

        CreateMaterialLocal(command, assets->reflectionProbeShader, reflectionProbeShader);
        command->ColorTexture = self->octTexture0;
        command->Color = self->Color;
        command->OverlayColor = self->OverlayColor;
        command->mesh = assets->sphereMesh;
        command->transform = self->transform;
        command->transform.scale = float3(0.25, 0.25, 0.25);
        Rendering::DrawMesh(command);

        if (haven->selectedEntity == (Entity*)self)
        {
            Debug::DrawBox(self->transform);
        }
    }

    if (!self->rendered)
    {
        ClearCubemaps();
        Rendering::RenderCubemap(self->transform.position, self->cubeTexture);
        Rendering::PackCubemap(self->octTexture0, self->cubeTexture);
        Rendering::Downsize4x(self->octTexture0, self->octTexture1, 20);
        Rendering::Downsize4x(self->octTexture1, self->octTexture2, 50);
        Rendering::Downsize4x(self->octTexture2, self->octTexture3, 400);
        Rendering::Downsize4x(self->octTexture3, self->octTexture4, 0);
        self->rendered = true;
    }
    SetCubemaps(); // update global cubemap assignments

}

#endif
