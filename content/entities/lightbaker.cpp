#pragma once
#include "../haven.cpp"

const int radiosityProbePosX = -1;
const int radiosityProbePosY = -1;
const int radiosityProbePosZ = -1;
const int radiosityProbeMultiplier = 2;
const int radiosityProbeCountX = 7 * radiosityProbeMultiplier;
const int radiosityProbeCountY = 12 * radiosityProbeMultiplier;
const int radiosityProbeCountZ = 7 * radiosityProbeMultiplier;
const float radiosityProbeScale = 1.0 / (float)radiosityProbeMultiplier;

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

    CustomRenderTexture* targetTexture;
    Transform transform;

    Texture* cubeTexture[6];
    Texture* blurDownsize0;
    Texture* blurDownsize1;
    Texture* blurDownsize2;
    Texture* blurDownsize3;
    Texture* blurDownsize4;
    bool rendering;
    int x;
    int y;
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
    ComposeTexturesMode_Overwrite,
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

LightBaker* LightBakerInstantiate()
{
    LightBaker* self = Instantiate(LightBaker);

    self->boxMin = float3(radiosityProbePosX, radiosityProbePosY, radiosityProbePosZ) + radiosityProbeScale * 0.5;
    self->count = float3(radiosityProbeCountX, radiosityProbeCountY, radiosityProbeCountZ);
    self->boxMax = self->boxMin + self->count * radiosityProbeScale;

    self->totalCount = radiosityProbeCountX * radiosityProbeCountY * radiosityProbeCountZ;

    self->targetTexture = CreateCustomRenderTexture(self->count.x * self->count.y, self->count.z, true);

    for (int i = 0; i < 6; i++)
    {
        self->cubeTexture[i] = CreateTextureTarget(64, 64, true);
    }
    self->blurDownsize0 = CreateTextureTarget(256, 256, true);
    self->blurDownsize1 = CreateTextureTarget(64, 64, true);
    self->blurDownsize2 = CreateTextureTarget(16, 16, true);
    self->blurDownsize3 = CreateTextureTarget(4, 4, true);
    self->blurDownsize4 = CreateTextureTarget(1, 1, true);
    return self;
}

float2 WorldPosToTexturePos(LightBaker* self, float3 worldPos)
{
    int x = (worldPos.x - self->boxMin.x) / radiosityProbeScale;
    int y = (worldPos.y - self->boxMin.y) / radiosityProbeScale;
    int z = (worldPos.z - self->boxMin.z) / radiosityProbeScale;
    return float2(x + (int)self->count.x * y, z);
}
float3 TexturePosToWorldPos(LightBaker* self, float2 texturePos)
{
    int x = texturePos.x;
    int y = texturePos.y;
    return float3(x, x % (int)self->count.x, y);
}

void RenderWorld(LightBaker* self, float3 pos, bool apply = true)
{
    //float3 pos = haven->spectatorCamera.position;
    RenderCubemap(pos, self->cubeTexture);
    PackCubemap(self->blurDownsize0, self->cubeTexture);
    Downsize4x(self->blurDownsize0, self->blurDownsize1);
    Downsize4x(self->blurDownsize1, self->blurDownsize2);
    Downsize4x(self->blurDownsize2, self->blurDownsize3);
    Downsize4x(self->blurDownsize3, self->blurDownsize4);
    if(apply)
        ComposeTextures(self->targetTexture, self->blurDownsize4, WorldPosToTexturePos(self, pos), ComposeTexturesMode_Overwrite);
}

#define crBegin static int state=0; switch(state) { case 0:
#define crReturn(i,x) do { state=i; return x; case i:; } while (0)
#define crFinish }


int testFunction(LightBaker* self)
{
    crBegin;
    for (self->x = 0; self->x < self->count.x; self->x++)
    {
        for (self->y = 0; self->y < self->count.y; self->y++)
        {
            for (int z = 0; z < self->count.z; z++)
            {
                haven->printf("test: x: %d, y: %d, z: %d\n", self->x, self->y, z);
                float3 pos = float3(self->x, self->y, z) * radiosityProbeScale + self->boxMin;
                RenderWorld(self, pos);
            }
            crReturn(1, self->x + self->y * self->count.x);
        }
    }
    crFinish;
    return -1;
}

void LightBakerUpdate(LightBaker* self)
{
    DrawAABBMinMax(self->boxMin, self->boxMax, 0.02);
    float3 boxCenter = (self->boxMin + self->boxMax) * 0.5;

    if (DrawButton("Radiosity"))
    {
        self->rendering = true;
        self->x = 0;
        self->y = 0;
    }
    if (self->rendering)
    {
        SetLightmap(haven->gameState,
            assets->black,
            self->boxMin,
            self->boxMax,
            self->count,
            radiosityProbeScale);
        int w = testFunction(self);
        haven->printf("test: w: %d\n", w);
        if (w == -1)
        {
            self->rendering = false;
        }
    }
    else
    {
        RenderWorld(self, haven->spectatorCamera.position, false);
    }

    //for (int x = 0; x < self->count.x; x++)
    //{
    //    for (int y = 0; y < self->count.y; y++)
    //    {
    //        for (int z = 0; z < self->count.z; z++)
    //        {
    //            float3 pos = float3(x, y, z) * radiosityProbeScale + self->boxMin;
    //            DrawPoint(pos, 0.05);
    //        }
    //    }
    //}

    SetLightmap(haven->gameState,
        self->targetTexture->current,
        self->boxMin,
        self->boxMax,
        self->count,
        radiosityProbeScale);

    DrawScreenTexture(self->targetTexture->current, self->targetTexture->current->size * 0.02 * radiosityProbeScale, 0.02);

    float scale = 0.3;
    DrawScreenTexture(self->blurDownsize0, float2(1, 1) * scale, 0 * 0.01 * scale * 4.0 - 0.05);
    DrawScreenTexture(self->blurDownsize1, float2(1, 1) * scale, 1 * 0.01 * scale * 4.0 - 0.05);
    DrawScreenTexture(self->blurDownsize2, float2(1, 1) * scale, 2 * 0.01 * scale * 4.0 - 0.05);
    DrawScreenTexture(self->blurDownsize3, float2(1, 1) * scale, 3 * 0.01 * scale * 4.0 - 0.05);
    DrawScreenTexture(self->blurDownsize4, float2(1, 1) * scale, 4 * 0.01 * scale * 4.0 - 0.05);
}
