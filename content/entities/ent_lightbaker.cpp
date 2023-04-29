
#include "../haven.cpp"
#include "ent_lightbaker.h"

#if structs
struct LightBaker
{
    float3 boxMin;
    float3 boxMax;
    float3 count;
    int totalCount;

    CustomRenderTexture* targetTexturePass0;
    CustomRenderTexture* targetTexturePass1;
    CustomRenderTexture* targetTexturePass2;
    Transform transform;

    Texture* cubeTexture[6];
    Texture* SphericalHarmonic;
    int pass;
    int x;
    int y;
    float radiosityProbeScale;
    int harmonicCount;
    int radiosityTextureSizeX;
    int radiosityTextureSizeY;

    Texture* bakedLighting;
    char bakedLightingFilename[100];
};

#else

LightBaker* LightBakerInstantiate(Texture* lightBake, char bakedLightingFilename[100], float3 boxMin, float3 boxSize)
{
    LightBaker* self = Instantiate(LightBaker);

    self->bakedLighting = lightBake;
    StringCopy(self->bakedLightingFilename, bakedLightingFilename);
    int radiosityProbePosX = (int)boxMin.x;
    int radiosityProbePosY = (int)boxMin.y;
    int radiosityProbePosZ = (int)boxMin.z;
    int radiosityProbeMultiplier = 2;
    int radiosityProbeCountX = (int)boxSize.x * radiosityProbeMultiplier;
    int radiosityProbeCountY = (int)boxSize.y * radiosityProbeMultiplier;
    int radiosityProbeCountZ = (int)boxSize.z * radiosityProbeMultiplier;
    self->radiosityProbeScale = 1.0 / (float)radiosityProbeMultiplier;
    self->harmonicCount = 9;
    self->radiosityTextureSizeX = radiosityProbeCountX * radiosityProbeCountY;
    self->radiosityTextureSizeY = radiosityProbeCountZ * self->harmonicCount;

    self->boxMin = float3(radiosityProbePosX, radiosityProbePosY, radiosityProbePosZ) + self->radiosityProbeScale * 0.5;
    self->count = float3(radiosityProbeCountX, radiosityProbeCountY, radiosityProbeCountZ);
    self->boxMax = self->boxMin + self->count * self->radiosityProbeScale;

    self->totalCount = radiosityProbeCountX * radiosityProbeCountY * radiosityProbeCountZ;

    self->targetTexturePass0 = Rendering::CreateCustomRenderTexture(self->count.x * self->count.y, self->count.z * self->harmonicCount, true);
    self->targetTexturePass1 = Rendering::CreateCustomRenderTexture(self->count.x * self->count.y, self->count.z * self->harmonicCount, true);
    self->targetTexturePass2 = Rendering::CreateCustomRenderTexture(self->count.x * self->count.y, self->count.z * self->harmonicCount, true);

    for (int i = 0; i < 6; i++)
    {
        self->cubeTexture[i] = Rendering::CreateTextureTarget(64, 64, true);
    }
    self->SphericalHarmonic = Rendering::CreateTextureTarget(1, self->harmonicCount, true);
    return self;
}

float2 WorldPosToLightmapTexturePos(LightBaker* self, float3 worldPos)
{
    int x = (worldPos.x - self->boxMin.x) / self->radiosityProbeScale;
    int y = (worldPos.y - self->boxMin.y) / self->radiosityProbeScale;
    int z = (worldPos.z - self->boxMin.z) / self->radiosityProbeScale;
    return float2(x + (int)self->count.x * y, z * self->harmonicCount);
}


void RenderWorld(LightBaker* self, float3 pos, CustomRenderTexture* target)
{
    //float3 pos = haven->spectatorCamera.position;
    Rendering::RenderCubemap(pos, self->cubeTexture);
    Rendering::CubemapToSphericalHarmonic(self->SphericalHarmonic, self->cubeTexture);
    //PackCubemap(self->blurDownsize0, self->cubeTexture);
    //Downsize4x(self->blurDownsize0, self->blurDownsize1);
    //Downsize4x(self->blurDownsize1, self->blurDownsize2);
    //Downsize4x(self->blurDownsize2, self->blurDownsize3);
    //Downsize4x(self->blurDownsize3, self->blurDownsize4);

    Rendering::ComposeTextures(target, self->SphericalHarmonic, WorldPosToLightmapTexturePos(self, pos), ComposeTexturesMode_Overwrite);
}

#define crBegin static int state=0; switch(state) { case 0:
#define crReturn(i,x) do { state=i; return x; case i:; } while (0)
#define crFinish }


int testFunction(LightBaker* self, CustomRenderTexture* target)
{
    crBegin;
    for (self->x = 0; self->x < self->count.x; self->x++)
    {
        for (self->y = 0; self->y < self->count.y; self->y++)
        {
            for (int z = 0; z < self->count.z; z++)
            {
                //haven->printf("test: x: %d, y: %d, z: %d\n", self->x, self->y, z);
                float3 pos = float3(self->x, self->y, z) * self->radiosityProbeScale + self->boxMin;
                RenderWorld(self, pos, target);
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
    if (Drawing::DrawButton("Bake Lighting"))
    {
        self->pass = 1;
        self->x = 0;
        self->y = 0;
        Rendering::DrawClear(self->targetTexturePass0);
        Rendering::DrawClear(self->targetTexturePass1);
        Rendering::DrawClear(self->targetTexturePass2);
    }
    if (self->pass == 1) // first pass
    {
        Rendering::SetLightmap(assets->black, self->boxMin, self->boxMax, self->count, self->radiosityProbeScale, 1);
        int w = testFunction(self, self->targetTexturePass0);
        if (w == -1)
        {
            self->pass++;
            self->x = 0;
            self->y = 0;
        }
        Rendering::SetLightmap(self->targetTexturePass0->current, self->boxMin, self->boxMax, self->count, self->radiosityProbeScale, 1);
    }
    else if (self->pass == 2) // 2nd pass
    {
        Rendering::SetLightmap(self->targetTexturePass0->current, self->boxMin, self->boxMax, self->count, self->radiosityProbeScale, 100);
        int w = testFunction(self, self->targetTexturePass1);
        if (w == -1)
        {
            self->pass++;
            self->x = 0;
            self->y = 0;
        }
        Rendering::SetLightmap(self->targetTexturePass1->current, self->boxMin, self->boxMax, self->count, self->radiosityProbeScale, 0.1);
    }
    else if (self->pass == 3) // 3rd pass
    {
        Rendering::SetLightmap(self->targetTexturePass1->current, self->boxMin, self->boxMax, self->count, self->radiosityProbeScale, 100);
        int w = testFunction(self, self->targetTexturePass2);
        if (w == -1)
        {
            self->pass++;
            self->x = 0;
            self->y = 0;
        }
        Rendering::SetLightmap(self->targetTexturePass2->current, self->boxMin, self->boxMax, self->count, self->radiosityProbeScale, 0.005);
    }
    else if(self->pass == 4) // done
    {
        self->pass++;

        // Done baking, write the result to disk.
        Texture* current = self->targetTexturePass2->current;

        // read the texture into the scratch buffer. Insert header containing the resolution before.
        Clear(haven->scratchBuffer, sizeof(haven->scratchBuffer));
        int headerSize = sizeof(uint64) * 2;
        ((uint64*)haven->scratchBuffer)[0] = self->radiosityTextureSizeX;
        ((uint64*)haven->scratchBuffer)[1] = self->radiosityTextureSizeY;
        haven->platformGraphicsReadbackTextureHDR(current, haven->scratchBuffer + headerSize);

        int NumberOfChannels = 4;
        int BytesPerChannel = 4;
        haven->platformWriteFile(haven->scratchBuffer, current->sizeX * current->sizeY * NumberOfChannels * BytesPerChannel + headerSize, self->bakedLightingFilename);
    }
    else
    {
        Rendering::SetLightmap(assets->bake_CornellBox, self->boxMin, self->boxMax, self->count, self->radiosityProbeScale, 0.005);
    }

    // draw SH bake texture
    bool debug = false;
    if (debug)
    {
        float scale = 0.005;
        DrawScreenTexture(self->targetTexturePass0->current, self->targetTexturePass0->current->size* scale* self->radiosityProbeScale, scale*1*5-0.05, float3(1, 1, 1));
        DrawScreenTexture(self->targetTexturePass1->current, self->targetTexturePass1->current->size* scale* self->radiosityProbeScale, scale*2*5-0.05, float3(0.1, 0.1, 0.1));
        DrawScreenTexture(self->targetTexturePass2->current, self->targetTexturePass2->current->size* scale* self->radiosityProbeScale, scale*3*5-0.05, float3(0.01, 0.01, 0.01));
    }


    Transform planeTransform = transform(float3(0,0,0),
        haven->spectatorCamera.right,
        haven->spectatorCamera.up,
        haven->spectatorCamera.forward,
        float3(1, 1, 1));

    CreateMaterialLocal(waterPlane2, assets->defaultlit, defaultlit);

    waterPlane2->texM1 = assets->baseM1;
    waterPlane2->texM2 = assets->baseM2;

    waterPlane2->texLightmap = self->targetTexturePass0->current;
    waterPlane2->Color = float3(1.0f, 1.0f, 1.0f);
    planeTransform.scale = -float3(0.15, 0.15, 0.15);

    planeTransform.position = haven->spectatorCamera.position + haven->spectatorCamera.forward;
    // float3(-0.2, 7.45, 1.6);
    //DrawMesh(waterPlane2, assets->sphere, planeTransform, "Probe plane in the scene");
}

#endif
