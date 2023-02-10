#pragma once
#include "../haven.cpp"
#include "reflectionProbe.h"

Camera PerspectiveCamera(Transform transform, float fov, float aspectRatio = 1.0, float maxDepth = 100.0f)
{
    Camera camera;
    camera.enabled = true;
    camera.fov = fov;
    camera.maxDepth = maxDepth;
    camera.orthographic = false;
    camera.orthoWidth = 10;
    camera.transform = transform;
    //camera.transform.position = startPos + float3(-0.5, 0, -0.3);
    camera.aspectRatio = aspectRatio;
    return camera;
}
Camera OrthoCamera(Transform transform, float orthoWidth, float aspectRatio = 1.0, float maxDepth = 100.0f)
{
    Camera camera;
    camera.enabled = true;
    camera.fov = 90;
    camera.maxDepth = maxDepth;
    camera.orthographic = true;
    camera.orthoWidth = orthoWidth;
    camera.transform = transform;
    //camera.transform.position = startPos + float3(-0.5, 0, -0.3);
    camera.aspectRatio = aspectRatio;
    return camera;
}

// assumes the texture target is set up already
void DrawScene(Camera camera)
{
    for (int j = 0; j < ArrayCount(haven->entities); j++)
    {
        Entity* entity = haven->entities[j];
        if (entity->type == EntityType_StaticMesh)
        {
            StaticMesh* mesh = (StaticMesh*)entity;
            if (CullMesh(mesh, camera.transform))
                continue;
            DrawStaticMesh(mesh, camera);
            //DrawMesh(mesh->material, mesh->mesh, mesh->transform, camera, "Scene StaticMesh");
        }
    }
}

void RenderOrtho(Transform transform, Texture* target, float orthoWidth, float aspectRatio, float maxDepth)
{
    SetRenderTarget(target, "Ortho Capture");
    DrawClear();
    DrawScene(OrthoCamera(transform, orthoWidth, aspectRatio, maxDepth));
    SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
}

void RenderCubemap(float3 startPos, Texture* cubeTexture[6])
{
    for (int i = 0; i < 6; i++)
    {
        if (!cubeTexture[i])
            return;
    }
    //float2 crossLocation[6]
    //{
    //    float2(1, 1),
    //    float2(-1, 1),
    //    float2(2, 1),
    //    float2(0, 1),
    //    float2(0, 0),
    //    float2(1, 3),
    //};

    Transform transforms[6] =
    {
        LookRotation(haven->spectatorCamera, float3( 1,  0,  0), float3(0, 0, 1)),
        LookRotation(haven->spectatorCamera, float3(-1,  0,  0), float3(0, 0, 1)),
        LookRotation(haven->spectatorCamera, float3( 0, -1,  0), float3(0, 0, 1)),
        LookRotation(haven->spectatorCamera, float3( 0,  1,  0), float3(0, 0, 1)),
        LookRotation(haven->spectatorCamera, float3( 0,  0, -1), float3(0, 1, 0)),
        LookRotation(haven->spectatorCamera, float3( 0,  0,  1), float3(0, 1, 0))
    };

    for (int i = 0; i < 6; i++)
    {
        SetRenderTarget(cubeTexture[i], "Probe Capture");
        if (i == 0) DrawClear(float3(1, 0, 0)); // 1, 0, 0
        if (i == 1) DrawClear(float3(0, 1, 1)); // 0, 1, 0
        if (i == 2) DrawClear(float3(1, 0, 1)); // 0, 0, 1
        if (i == 3) DrawClear(float3(0, 1, 0)); // 0, 1, 1
        if (i == 4) DrawClear(float3(1, 1, 0)); // 1, 0, 1
        if (i == 5) DrawClear(float3(0, 0, 1)); // 1, 1, 0
        DrawClear(haven->clearColor);

        Transform t = transforms[i];
        t.position = startPos;
        DrawScene(PerspectiveCamera(t, 90, 1, 100));
    }
    SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
}

void PackCubemap(Texture* target, Texture* source[6])
{
    if (!target)
        return;

    for (int i = 0; i < 6; i++)
    {
        if (!source[i])
            return;
    }
    SetRenderTarget(target, "Probe Capture");
    DrawClear(float3(0, 1, 0));
    CreateMaterialLocal(octUnwrap, assets->reflectionProbeCubemapToOct, reflectionProbeCubemapToOct);
    octUnwrap->cubeTexture0 = source[0];
    octUnwrap->cubeTexture1 = source[1];
    octUnwrap->cubeTexture2 = source[2];
    octUnwrap->cubeTexture3 = source[3];
    octUnwrap->cubeTexture4 = source[4];
    octUnwrap->cubeTexture5 = source[5];
    octUnwrap->mesh = assets->ui_quad;
    DrawMesh(octUnwrap);

    SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
}

// 5x5 tap blur, good for dropping texture resolution by 4x.
void Downsize4x(Texture* source, Texture* target)
{
    if (!source)
        return;
    if (!target)
        return;

    SetRenderTarget(target, "Probe Capture");
    DrawClear(float3(0, 1, 0));
    CreateMaterialLocal(octUnwrap, assets->downsize4x, downsize4x);
    octUnwrap->colorTexture = source;
    octUnwrap->resolution = target->sizeX;
    octUnwrap->mesh = assets->ui_quad;
    DrawMesh(octUnwrap);
    SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
}
struct ReflectionProbe
{
    EntityContents;
    Texture* cubeTexture[6];
    Texture* octTexture;
};

ReflectionProbe* ReflectionProbeInstantiate(Transform transform)
{
    ReflectionProbe* self = Instantiate(ReflectionProbe);

    for (int i = 0; i < 6; i++)
    {
        self->cubeTexture[i] = CreateTextureTarget(1024, 1024, true);
    }
    self->octTexture = CreateTextureTarget(1024, 1024, true);

    self->transform = transform;
    return self;
}

void DrawScreenTexture(Texture* texture, float2 size, float heightOffset)
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
    CreateMaterialLocal(waterPlane, assets->unlit, unlit);
    waterPlane->ColorTexture = texture;
    waterPlane->Color = float3(1.0f, 1.0f, 1.0f);
    waterPlane->BackFaceCulling = true;
    DrawMesh(waterPlane, assets->ui_quad, planeTransform, "Probe plane in the scene");
}
void ReflectionProbeUpdate(ReflectionProbe* self, int i)
{
    return;
    float3 startPos = haven->spectatorCamera.position + float3(-0.5, 2, -0.5);
    RenderCubemap(startPos, self->cubeTexture);
    PackCubemap(self->octTexture, self->cubeTexture);

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

