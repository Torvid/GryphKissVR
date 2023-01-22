#pragma once
#include "../haven.cpp"
#include "reflectionProbe.h"

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

void ReflectionProbeUpdate(ReflectionProbe* self, int i)
{
    float3 startPos = haven->spectatorCamera.position + float3(-0.5, 2, -0.5);
    //Transform planeTransform = transform(haven->spectatorCamera.position + float3(-0.5, 2, -0.5));
    //planeTransform.scale = float3(0.25, 0.25, 0.25);
    Transform transforms[6] =
    {
        LookRotation(haven->spectatorCamera, float3( 1,  0,  0), float3(0, 0, 1)),
        LookRotation(haven->spectatorCamera, float3(-1,  0,  0), float3(0, 0, 1)),
        LookRotation(haven->spectatorCamera, float3(0, -1,  0), float3(0, 0, 1)),
        LookRotation(haven->spectatorCamera, float3(0,  1,  0), float3(0, 0, 1)),
        LookRotation(haven->spectatorCamera, float3( 0,  0, -1), float3(0, 1, 0)),
        LookRotation(haven->spectatorCamera, float3(0,  0,  1), float3(0, 1, 0))
    };

    float2 screenLocation[6]
    {
        float2(1, 1),
        float2(-1, 1),
        float2(2, 1),
        float2(0, 1),
        float2(0, 0),
        float2(1, 3),
    };
    Transform planeTransform;

    for (int i = 0; i < 6; i++)
    {

        SetRenderTarget(self->cubeTexture[i], "Probe Capture");
        if (i == 0) DrawClear(float3(1, 0, 0)); // 1, 0, 0
        if (i == 1) DrawClear(float3(0, 1, 1)); // 0, 1, 0
        if (i == 2) DrawClear(float3(1, 0, 1)); // 0, 0, 1
        if (i == 3) DrawClear(float3(0, 1, 0)); // 0, 1, 1
        if (i == 4) DrawClear(float3(1, 1, 0)); // 1, 0, 1
        if (i == 5) DrawClear(float3(0, 0, 1)); // 1, 1, 0
        DrawClear(haven->clearColor);
        for (int j = 0; j < ArrayCount(haven->entities); j++)
        {
            Entity* entity = haven->entities[j];
            if (entity->type == EntityType_StaticMesh)
            {
                StaticMesh* mesh = (StaticMesh*)entity;
                Camera camera;
                camera.enabled = true;
                camera.fov = 90;
                camera.maxDepth = 2;
                camera.orthographic = false;
                camera.orthoWidth = 10;
                camera.transform = transforms[i];
                camera.transform.position = startPos + float3(-0.5, 0, -0.3);// haven->spectatorCamera.position;
                camera.aspectRatio = 1.0f;
                DrawMesh(mesh->material, mesh->mesh, mesh->transform, camera, false, "Scene StaticMesh");
            }
        }
        
        float size = 0.5;
        SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
        planeTransform = transform(startPos + float3(screenLocation[i].x, 0, screenLocation[i].y) * size);
        planeTransform.scale = float3(size, size, size);
        planeTransform = LookRotation(planeTransform, float3(0, 0, 1), float3(0, -1, 0));
        if(i==5)
            planeTransform = LookRotation(planeTransform, float3(0, 0, -1), float3(0, -1, 0));
        
        
        CreateMaterialLocal(waterPlane, unlit);
        waterPlane->ColorTexture = self->cubeTexture[i];
        waterPlane->Color = float3(1.0f, 1.0f, 1.0f);
        waterPlane->BackFaceCulling = true;
        DrawMesh(waterPlane, assets->ui_quad, planeTransform, "Probe plane in the scene");
        if(i == 4)
            DrawFont("Cube packing: ", transform(planeTransform.position), 1, 2.0f, HAlign_left, VAlign_up);
    }



    SetRenderTarget(self->octTexture, "Probe Capture");
    DrawClear(float3(0, 1, 0));
    CreateMaterialLocal(octUnwrap, reflectionProbeCubemapToOct);
    octUnwrap->cubeTexture0 = self->cubeTexture[0];
    octUnwrap->cubeTexture1 = self->cubeTexture[1];
    octUnwrap->cubeTexture2 = self->cubeTexture[2];
    octUnwrap->cubeTexture3 = self->cubeTexture[3];
    octUnwrap->cubeTexture4 = self->cubeTexture[4];
    octUnwrap->cubeTexture5 = self->cubeTexture[5];
    octUnwrap->mesh = assets->ui_quad;
    DrawMesh(octUnwrap);

    SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");

    // draw quad
    float size = 0.75;
    planeTransform = transform(startPos + float3(1.5, 0, -0.5) * size);
    planeTransform.scale = float3(size, size, size);
    planeTransform = LookRotation(planeTransform, float3(0, 0, 1), float3(0, -1, 0));
    CreateMaterialLocal(waterPlane, unlit);
    waterPlane->ColorTexture = self->octTexture;
    waterPlane->Color = float3(1.0f, 1.0f, 1.0f);
    waterPlane->BackFaceCulling = true;
    DrawMesh(waterPlane, assets->ui_quad, planeTransform, "Probe plane in the scene");

    DrawFont("Octahedral packing: ", transform(planeTransform.position), 1, 2.0f, HAlign_left, VAlign_up);

    CreateMaterialLocal(waterPlane2, defaultlit);
    waterPlane2->texM1 = assets->baseM1;
    waterPlane2->texM2 = assets->baseM2;
    waterPlane2->texCubemap = self->octTexture;
    waterPlane2->Color = float3(1.0f, 1.0f, 1.0f);
    planeTransform.scale = float3(0.25, 0.25, 0.25);
    planeTransform.position = startPos + float3(-0.5, 0, -0.3);
    DrawMesh(waterPlane2, assets->sphereMesh, planeTransform, "Probe plane in the scene");
}

