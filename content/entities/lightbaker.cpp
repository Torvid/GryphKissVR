#pragma once
#include "../haven.cpp"

const int radiosityProbePosX = -1;
const int radiosityProbePosY = -1;
const int radiosityProbePosZ = -1;
const int radiosityProbeCountX = 7 * 2;
const int radiosityProbeCountY = 12 * 2;
const int radiosityProbeCountZ = 7 * 2;
const float radiosityProbeScale = 0.5;

struct CustomRenderTexture
{
    Texture* current;
    Texture* previous;
};
CustomRenderTexture* CreateCustomRenderTexture(int sizeX, int sizeY, bool clamp = false)
{
    CustomRenderTexture* result = ArenaPushStruct(&haven->arenaEngineState, CustomRenderTexture, "CustomRenderTexture");
    result->current = CreateTextureTarget(sizeX, sizeY, clamp);
    result->previous = CreateTextureTarget(sizeX, sizeY, clamp);
    return result;
}
void Swap(CustomRenderTexture* texture)
{
    Texture* temp = texture->current;
    texture->current = texture->previous;
    texture->previous = temp;
}
void DrawClear(CustomRenderTexture* texture)
{
    SetRenderTarget(texture->current);
    DrawClear();
    SetRenderTarget(texture->previous);
    DrawClear();
    SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
}

struct LightBaker
{
    float3 boxMin;
    float3 boxMax;
    float3 count;
    int totalCount;

    Texture* xAxis;
    Texture* yAxis;
    Texture* zAxis;
    CustomRenderTexture* testTextureX;
    CustomRenderTexture* testTextureY;
    CustomRenderTexture* testTextureZ;
    Transform transform;
};

enum ComposeTexturesMode
{
    ComposeTexturesMode_Add,
    ComposeTexturesMode_Subtract,
    ComposeTexturesMode_Multiply,
    ComposeTexturesMode_Divide,
    ComposeTexturesMode_Max,
    ComposeTexturesMode_Min,
    ComposeTexturesMode_IfNotZero,
};

void ComposeTextures(CustomRenderTexture* target, Texture* source, float2 pos, ComposeTexturesMode composeMode)
{
    float2 targetSize = float2(target->current->sizeX, target->current->sizeY);
    float2 sourceSize = float2(source->sizeX, source->sizeY);

    Swap(target);
    CreateMaterialLocal(comp, compose);
    comp->mesh = assets->ui_quad;
    comp->texA = target->previous; 
    comp->texB = source;
    comp->Min = (pos) / targetSize;
    comp->Max = (pos + sourceSize) / targetSize;
    comp->composeMode = (int)composeMode;
    SetRenderTarget(target->current);
    DrawClear(float3(0, 0, 1));
    DrawMesh(comp);
    SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
}

LightBaker* LightBakerStart()
{
    LightBaker* self = Instantiate(LightBaker);

    self->boxMin = float3(radiosityProbePosX, radiosityProbePosY, radiosityProbePosZ);// +radiosityProbeScale * 0.5 + 0.5;
    self->count = float3(radiosityProbeCountX, radiosityProbeCountY, radiosityProbeCountZ);
    self->boxMax = self->boxMin + self->count * radiosityProbeScale;

    self->totalCount = radiosityProbeCountX * radiosityProbeCountY * radiosityProbeCountZ;

    self->testTextureX = CreateCustomRenderTexture(self->count.x * self->count.y, self->count.z, true);
    self->testTextureY = CreateCustomRenderTexture(self->count.x * self->count.y, self->count.z, true);
    self->testTextureZ = CreateCustomRenderTexture(self->count.x * self->count.z, self->count.y, true);
    self->xAxis = CreateTextureTarget(self->count.y, self->count.z, true);
    self->yAxis = CreateTextureTarget(self->count.x, self->count.z, true);
    self->zAxis = CreateTextureTarget(self->count.x, self->count.y, true);
    return self;
}

void LightBakerUpdate(LightBaker* self)
{
    DrawAABBMinMax(self->boxMin, self->boxMax, 0.02);
    float3 boxCenter = (self->boxMin + self->boxMax) * 0.5;

    DrawScreenTexture(self->testTextureX->current, self->testTextureX->current->size * 0.01, 0);
    DrawScreenTexture(self->testTextureY->current, self->testTextureY->current->size * 0.01, 0.01);
    DrawScreenTexture(self->testTextureZ->current, self->testTextureZ->current->size * 0.01, 0.02);

    DrawClear(self->testTextureX);
    float3 startX = float3(self->boxMin.x, boxCenter.y, boxCenter.z);
    for (int i = 0; i < self->count.x; i++)
    {
        DrawPoint(startX, 0.1, i == 0 ? float3(1, 0, 0) : float3(1, 1, 1));
        Transform t = haven->spectatorCamera;

        //t = transform(startX, float3(0, 1, 0), float3(1, 0, 0), float3(0, 0, 1), float3(1, 1, 1));
        RenderOrtho(t, self->xAxis, self->xAxis->sizeX * radiosityProbeScale, self->xAxis->aspectRatio, radiosityProbeScale);
        ComposeTextures(self->testTextureX, self->xAxis, float2(i * self->xAxis->sizeX, 0), ComposeTexturesMode_IfNotZero);

        startX += float3(radiosityProbeScale, 0, 0);

        //t = transform(startX, float3(0, -1, 0), float3(1, 0, 0), float3(0, 0, 1), float3(1, 1, 1));
        RenderOrtho(t, self->xAxis, self->xAxis->sizeX * radiosityProbeScale, self->xAxis->aspectRatio, radiosityProbeScale);
        ComposeTextures(self->testTextureX, self->xAxis, float2(i * self->xAxis->sizeX, 0), ComposeTexturesMode_IfNotZero);
    }

    if (DrawButton("Radiosity"))
    {
        DrawClear(self->testTextureY);
        float3 startY = float3(boxCenter.x, self->boxMin.y, boxCenter.z);
        for (int i = 0; i < self->count.y; i++)
        {
            DrawPoint(startY, 0.1, i == 0 ? float3(1, 0, 0) : float3(1, 1, 1));
            Transform t;

            t = transform(startY, float3(1, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
            RenderOrtho(t, self->yAxis, self->yAxis->sizeX * radiosityProbeScale, self->yAxis->aspectRatio, radiosityProbeScale);
            ComposeTextures(self->testTextureY, self->yAxis, float2(i * self->yAxis->sizeX, 0), ComposeTexturesMode_IfNotZero);

            startY += float3(0, radiosityProbeScale, 0);

            t = transform(startY, float3(-1, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
            RenderOrtho(t, self->yAxis, self->yAxis->sizeX * radiosityProbeScale, self->yAxis->aspectRatio, radiosityProbeScale);
            ComposeTextures(self->testTextureY, self->yAxis, float2(i * self->yAxis->sizeX, 0), ComposeTexturesMode_IfNotZero);
        }

    }
}
