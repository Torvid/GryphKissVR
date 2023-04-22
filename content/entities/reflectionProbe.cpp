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
        self->cubeTexture[i] = Rendering::CreateTextureTarget(1024, 1024, true);
    }
    self->octTexture = Rendering::CreateTextureTarget(1024, 1024, true);

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
    Rendering::DrawMesh(waterPlane, assets->ui_quad, planeTransform, "Probe plane in the scene");
}
void ReflectionProbeUpdate(ReflectionProbe* self, int i)
{
    return;
    float3 startPos = haven->spectatorCamera.position + float3(-0.5, 2, -0.5);
    Rendering::RenderCubemap(startPos, self->cubeTexture);
    Rendering::PackCubemap(self->octTexture, self->cubeTexture);

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

