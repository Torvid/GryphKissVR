#pragma once
#include "../haven.cpp"

const int radiosityProbePosX = -1;
const int radiosityProbePosY = -1;
const int radiosityProbePosZ = -1;
const int radiosityProbeCountX = 7 * 2;
const int radiosityProbeCountY = 12 * 2;
const int radiosityProbeCountZ = 7 * 2;
const float radiosityProbeScale = 0.5;

struct LightBaker
{
    float3 boxMin;
    float3 boxMax;
    float3 count;
    int totalCount;

    Texture* dataTexture;
    Texture* xAxis;
    Texture* yAxis;
    Texture* zAxis;
};

LightBaker* LightBakerStart()
{
    LightBaker* self = Instantiate(LightBaker);

    self->boxMin = float3(radiosityProbePosX, radiosityProbePosY, radiosityProbePosZ);// +radiosityProbeScale * 0.5 + 0.5;
    self->count = float3(radiosityProbeCountX, radiosityProbeCountY, radiosityProbeCountZ);
    self->boxMax = self->boxMin + self->count * radiosityProbeScale;

    self->totalCount = radiosityProbeCountX * radiosityProbeCountY * radiosityProbeCountZ;

    self->dataTexture = CreateTextureTarget(self->count.x, self->count.y * self->count.z, true);
    self->xAxis = CreateTextureTarget(self->count.x, self->count.z, true);
    self->yAxis = CreateTextureTarget(self->count.y, self->count.z, true);
    self->zAxis = CreateTextureTarget(self->count.x, self->count.y, true);
    return self;
}



void LightBakerUpdate(LightBaker* self)
{
    for (int i = 0; i < self->count.z; i++)
    {
        RenderOrtho(haven->spectatorCamera, self->xAxis, self->xAxis->width * radiosityProbeScale, self->xAxis->aspectRatio, radiosityProbeScale);
    }

    DrawPoint(self->boxMin, 0.1, float3(1,0,0));
    DrawScreenTexture(self->xAxis);
    DrawAABBMinMax(self->boxMin, self->boxMax, 0.02);
    if (DrawButton("Radiosity"))
    {
    }
}
