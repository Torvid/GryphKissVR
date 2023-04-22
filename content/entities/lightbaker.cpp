#pragma once
#include "../haven.cpp"
#include "lightbaker.h"

struct LightBaker
{
    float3 boxMin;
    float3 boxMax;
    float3 count;
    int totalCount;

    CustomRenderTexture* targetTexture;
    Transform transform;

    Texture* cubeTexture[6];
    Texture* SphericalHarmonic;
    bool rendering;
    int x;
    int y;
};

LightBaker* LightBakerInstantiate()
{
    LightBaker* self = Instantiate(LightBaker);

    self->boxMin = float3(radiosityProbePosX, radiosityProbePosY, radiosityProbePosZ) + radiosityProbeScale * 0.5;
    self->count = float3(radiosityProbeCountX, radiosityProbeCountY, radiosityProbeCountZ);
    self->boxMax = self->boxMin + self->count * radiosityProbeScale;

    self->totalCount = radiosityProbeCountX * radiosityProbeCountY * radiosityProbeCountZ;

    self->targetTexture = Rendering::CreateCustomRenderTexture(self->count.x * self->count.y, self->count.z * harmonicCount, true);

    for (int i = 0; i < 6; i++)
    {
        self->cubeTexture[i] = Rendering::CreateTextureTarget(64, 64, true);
    }
    self->SphericalHarmonic = Rendering::CreateTextureTarget(1, harmonicCount, true);
    return self;
}

float2 WorldPosToLightmapTexturePos(LightBaker* self, float3 worldPos)
{
    int x = (worldPos.x - self->boxMin.x) / radiosityProbeScale;
    int y = (worldPos.y - self->boxMin.y) / radiosityProbeScale;
    int z = (worldPos.z - self->boxMin.z) / radiosityProbeScale;
    return float2(x + (int)self->count.x * y, z * harmonicCount);
}


void RenderWorld(LightBaker* self, float3 pos, bool apply = true)
{
    //float3 pos = haven->spectatorCamera.position;
    Rendering::RenderCubemap(pos, self->cubeTexture);
    Rendering::CubemapToSphericalHarmonic(self->SphericalHarmonic, self->cubeTexture);
    //PackCubemap(self->blurDownsize0, self->cubeTexture);
    //Downsize4x(self->blurDownsize0, self->blurDownsize1);
    //Downsize4x(self->blurDownsize1, self->blurDownsize2);
    //Downsize4x(self->blurDownsize2, self->blurDownsize3);
    //Downsize4x(self->blurDownsize3, self->blurDownsize4);
    if(apply)
        Rendering::ComposeTextures(self->targetTexture, self->SphericalHarmonic, WorldPosToLightmapTexturePos(self, pos), ComposeTexturesMode_Overwrite);
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
                //haven->printf("test: x: %d, y: %d, z: %d\n", self->x, self->y, z);
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
    Drawing::DrawAABBMinMax(self->boxMin, self->boxMax, 0.02);
    float3 boxCenter = (self->boxMin + self->boxMax) * 0.5;

    Rendering::DrawClear(self->SphericalHarmonic);
    if (Drawing::DrawButton("Radiosity"))
    {
        self->rendering = true;
        self->x = 0;
        self->y = 0;
    }
    if (self->rendering)
    {
        Rendering::SetLightmap(
            assets->black,
            self->boxMin,
            self->boxMax,
            self->count,
            radiosityProbeScale);
        int w = testFunction(self);
        //haven->printf("test: w: %d\n", w);
        if (w == -1)
        {
            self->rendering = false;

            // Done baking, write the result to disk.
            Texture* current = self->targetTexture->current;

            // read the texture into the scratch buffer
            Clear(haven->scratchBuffer, sizeof(haven->scratchBuffer));
            haven->platformGraphicsReadbackTextureHDR(current, haven->scratchBuffer);
            //for (int y = 0; y < current->sizeY; y++)
            //{
            //    for (int x = 0; x < current->sizeX; x++)
            //    {
            //        int i = (y * current->sizeX + x) * 4;
            //        float* currentPixel = (float*)haven->scratchBuffer + i;
            //
            //        // RGBA TO BGRA
            //        float R = *(currentPixel + 0);
            //        float G = *(currentPixel + 1);
            //        float B = *(currentPixel + 2);
            //        float A = *(currentPixel + 3);
            //
            //        *(currentPixel + 0) = B;
            //        *(currentPixel + 1) = G;
            //        *(currentPixel + 2) = R;
            //        *(currentPixel + 3) = A;
            //    }
            //}
            int NumberOfChannels = 4;
            int BytesPerChannel = 4;
            haven->platformWriteFile(haven->scratchBuffer, current->sizeX * current->sizeY * NumberOfChannels * BytesPerChannel, "LightBake.rad");
        }
        Rendering::SetLightmap(
            self->targetTexture->current,
            self->boxMin,
            self->boxMax,
            self->count,
            radiosityProbeScale);
    }
    else
    {
        //RenderWorld(self, haven->spectatorCamera.position, false);
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

    // draw SH bake texture
    bool debug = false;
    if (debug)
    {
        DrawScreenTexture(self->targetTexture->current, self->targetTexture->current->size * 0.015 * radiosityProbeScale, 0.015);
        DrawScreenTexture(assets->LightBake, assets->LightBake->size * 0.015 * radiosityProbeScale, -0.05);
    }
    
    //float scale = 1.0;
    //DrawScreenTexture(self->SphericalHarmonic, float2(1, 1) * scale, 0 * 0.01 * scale * 4.0 - 0.05);
    //DrawScreenTexture(self->blurDownsize1, float2(1, 1) * scale, 1 * 0.01 * scale * 4.0 - 0.05);
    //DrawScreenTexture(self->blurDownsize2, float2(1, 1) * scale, 2 * 0.01 * scale * 4.0 - 0.05);
    //DrawScreenTexture(self->blurDownsize3, float2(1, 1) * scale, 3 * 0.01 * scale * 4.0 - 0.05);
    //DrawScreenTexture(self->blurDownsize4, float2(1, 1) * scale, 4 * 0.01 * scale * 4.0 - 0.05);


    //if (DrawButton("Load Baked data"))
    //{
    //    //Texture* texture;
    //    //LoadTexture(texture)
    //}
    if (!self->rendering)
    {
        Rendering::SetLightmap(
            assets->LightBake,
            self->boxMin,
            self->boxMax,
            self->count,
            radiosityProbeScale);
    }

    Transform planeTransform = transform(float3(0,0,0),
        haven->spectatorCamera.right,
        haven->spectatorCamera.up,
        haven->spectatorCamera.forward,
        float3(1, 1, 1));

    CreateMaterialLocal(waterPlane2, assets->defaultlit, defaultlit);
    //waterPlane2->BackFaceCulling = false;
    waterPlane2->texM1 = assets->baseM1;
    waterPlane2->texM2 = assets->baseM2;
    //waterPlane2->texM1 = assets->BarnWallM1;
    //waterPlane2->texM2 = assets->BarnWallM2;
    //waterPlane2->texCubemap = self->octTexture;
    waterPlane2->texLightmap = self->targetTexture->current;
    waterPlane2->Color = float3(1.0f, 1.0f, 1.0f);
    planeTransform.scale = -float3(0.15, 0.15, 0.15);

    planeTransform.position = haven->spectatorCamera.position + haven->spectatorCamera.forward;
    // float3(-0.2, 7.45, 1.6);
    //DrawMesh(waterPlane2, assets->sphere, planeTransform, "Probe plane in the scene");
}
