#pragma once
#include "../haven.cpp"

struct ReflectionProbe
{
    EntityContents;
    Texture targetTexture;
};

ReflectionProbe* ReflectionProbeInstantiate(Transform transform)
{
    ReflectionProbe* self = Instantiate(ReflectionProbe);

    self->targetTexture = CreateTextureTarget(512, 512);
    self->transform = transform;
    return self;
}

void ReflectionProbeUpdate(ReflectionProbe* self, int i)
{
    SetRenderTarget(self->targetTexture);
    DrawClear();

    SetRenderTarget(0);

    //Transform identity = Transform(vectorDown * 3.0, vectorForward, vectorUp, vectorOne);
    CreateMaterialLocal(waterPlane, assets->unlit, Material_unlit);
    waterPlane->ColorTexture = self->targetTexture;// haven->waterRipplesCurrent;
    waterPlane->Color = float3(1.0f, 1.0f, 1.0f);
    //waterPlane->mesh = assets->ui_quad;
    waterPlane->BackFaceCulling = true;
    //waterPlane->transform = identity;
    DrawMesh(waterPlane, assets->ui_quad, transform(vectorDown * 2.0));
}

