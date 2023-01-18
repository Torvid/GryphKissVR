#pragma once
#include "../haven.cpp"

//const float3 radiosityBoxMin = { -2, -1, -1 };
//const float3 radiosityBoxMax = { 8, 12, 7 };
//const float3 radiosityBoxSize = { 6, 11, 6 };
const int radiosityProbePosX = -1;
const int radiosityProbePosY = -1;
const int radiosityProbePosZ = -1;
const int radiosityProbeCountX = 6 * 2;
const int radiosityProbeCountY = 11 * 2;
const int radiosityProbeCountZ = 6 * 2;
const float radiosityProbeScale = 0.5;

struct RadiosityProbe
{
    float3 color;
    float3 location;
    float occlusion[radiosityProbeCountZ][radiosityProbeCountY][radiosityProbeCountX];
    bool canSeeSky;
};

struct LightBaker
{
    MemoryArena radiosityArena;
    RadiosityProbe probes[radiosityProbeCountZ][radiosityProbeCountY][radiosityProbeCountX];
    float3 boxMin;
    float3 boxMax;
    float3 count;
    int totalCount;
};

LightBaker* LightBakerStart()
{
    LightBaker* self = Instantiate(LightBaker);

    ArenaInitialize(&self->radiosityArena, Megabytes(64), (uint8*)ArenaPushBytes(&haven->arenasArena, Megabytes(64), "Radiosity Probes", true), "Radiosity Probes");
    self->boxMin = float3(radiosityProbePosX, radiosityProbePosY, radiosityProbePosZ) + radiosityProbeScale * 0.5 + 0.5;
    self->count = float3(radiosityProbeCountX, radiosityProbeCountY, radiosityProbeCountZ);
    self->boxMax = self->boxMin + self->count * radiosityProbeScale;

    self->totalCount = radiosityProbeCountX * radiosityProbeCountY * radiosityProbeCountZ;

    return self;
}

bool worldRaycast(float3 pos0, float3 pos1)
{
    for (int k = 0; k < ArrayCount(haven->entities); k++)
    {
        if (haven->entities[k]->type == EntityType_StaticMesh)
        {
            StaticMesh* mesh = (StaticMesh*)haven->entities[k];
            Transform t = GetLocalBoundsTransform(mesh);
            float3 startLocal = WorldToLocal(pos0, t);
            float3 endLocal = WorldToLocal(pos1, t);
            if (RayBoxIntersect(startLocal, endLocal))
            {
                return true;
            }
        }
    }
    return false;
}

void LightBakerUpdate(LightBaker* self)
{
    if (!haven->editor)
        return;

    //for (int i = 0; i < self->count; i++)
    //{
    //    float3 color = self->radiosityProbeColors[i];
    //    float3 position = self->radiosityProbeLocation[i];
    //    DrawPoint(position, 0.05, color);
    //}
    float3 TestPoint = float3(0, 0, 0);
    DrawSphere(TestPoint + self->boxMin, 0.15f);
    for (int x = 0; x < self->count.x; x++)
    {
        for (int y = 0; y < self->count.y; y++)
        {
            for (int z = 0; z < self->count.z; z++)
            {
                RadiosityProbe* probe = &self->probes[x][y][z];
                float color = probe->canSeeSky;
                //float dist = self->probes[4][4][4].occlusion[x][y][z];
                //DrawPoint(float3(x, y, z) + self->boxMin, 0.05, float3(dist, dist, dist)*0.1);
                //DrawPoint(float3(x, y, z) * radiosityProbeScale + self->boxMin, 0.05, float3(color, color, color));
                DrawPoint(float3(x, y, z) * radiosityProbeScale + self->boxMin, 0.05, probe->color);
            }
        }
    }

    //float3 start = haven->spectatorCamera.position + haven->spectatorCamera.forward;//float3(-4, 10, 0.5);
    //float3 end = float3(1.1, -1, 0.51);
    //DrawLine(start, end, 0.01f, float3(1, 1, 1));
    //for (int k = 0; k < ArrayCount(haven->entities); k++)
    //{
    //    if (haven->entities[k]->type == "StaticMesh")
    //    {
    //        StaticMesh* mesh = (StaticMesh*)haven->entities[k];
    //        Transform t = GetLocalBoundsTransform(mesh);
    //        float3 startLocal = WorldToLocal(t, start);
    //        float3 endLocal = WorldToLocal(t, end);
    //        if (RayBoxIntersect(startLocal, endLocal))
    //            DrawBox(t, 0.01, float3(1, 0, 0));
    //        else
    //            DrawBox(t, 0.01, float3(1, 1, 1));
    //    }
    //}
    float3 sunDirection = normalize(float3(0.76, 0.3, -0.5));

    float3 sunColor = float3(1, 1, 1);

    for (int x = 0; x < self->count.x; x++)
    {
        for (int y = 0; y < self->count.y; y++)
        {
            for (int z = 0; z < self->count.z; z++)
            {
                float3 pos0 = float3(x, y, z) * radiosityProbeScale + self->boxMin;
                //self->probes[x][y][z].canSeeSky = worldRaycast(pos0, pos0 - sunDirection * 99);
                //DrawLine(pos0, pos0 - sunDirection * 99);
            }
        }
    }
    int offset = 4;

    if (DrawButton("Lighting"))
    {
        // Inject lighting
        for (int x = 0; x < self->count.x; x++)
        {
            for (int y = 0; y < self->count.y; y++)
            {
                for (int z = 0; z < self->count.z; z++)
                {
                    RadiosityProbe* sourceProbe = &self->probes[x][y][z];
                    sourceProbe->color = float3(0, 0, 0);
                    if (sourceProbe->canSeeSky)
                    {
                        sourceProbe->color += sunColor;
                    }
                }
            }
        }

        for (int i = 0; i < 3; i++)
        {

            for (int x = 0; x < self->count.x; x++)
            {
                for (int y = 0; y < self->count.y; y++)
                {
                    for (int z = 0; z < self->count.z; z++)
                    {
                        RadiosityProbe* sourceProbe = &self->probes[x][y][z];

                        for (int xx = (-offset + x); xx < (offset + 1 + x); xx++)
                        {
                            if (xx < 0 || xx >= self->count.x)
                                continue;
        
                            for (int yy = (-offset + y); yy < (offset + 1 + y); yy++)
                            {
                                if (yy < 0 || yy >= self->count.y)
                                    continue;
        
                                for (int zz = (-offset + z); zz < (offset + 1 + z); zz++)
                                {
                                    if (zz < 0 || zz >= self->count.z)
                                        continue;

                                    float maxDistance = offset;
                                    RadiosityProbe* targetProbe = &self->probes[xx][yy][zz];
                                    float dist = sourceProbe->occlusion[xx][yy][zz];

                                    float falloff = (maxDistance - dist) / maxDistance;// clamp01();
                                    falloff = 1.0f / (dist + 1.0);
                                    targetProbe->color += sourceProbe->color* falloff*0.005;// *falloff;
                                }
                            }
                        }
        
                    }
                }
            }
        }
    }
    if (DrawButton("Radiosity"))
    {
        haven->printf("Baking lighting ...");
        // clear
        for (int x = 0; x < self->count.x; x++)
        {
            for (int y = 0; y < self->count.y; y++)
            {
                for (int z = 0; z < self->count.z; z++)
                {
                    self->probes[x][y][z].color = float3(0, 0, 0);

                    for (int xx = 0; xx < self->count.x; xx++)
                    {
                        for (int yy = 0; yy < self->count.y; yy++)
                        {
                            for (int zz = 0; zz < self->count.z; zz++)
                            {
                                self->probes[x][y][z].occlusion[xx][yy][zz] = 999;
                            }
                        }
                    }
                }
            }
        }


        // raycast and cache collisions
        for (int x = 0; x < self->count.x; x++)
        {
            haven->printf("layer X %d/%d\n", x, ((int)self->count.x));
            for (int y = 0; y < self->count.y; y++)
            {
                for (int z = 0; z < self->count.z; z++)
                {
                    //int x = 1;// + self->boxMin.x;//x1 - self->boxMin.x;
                    //int y = 1;// + self->boxMin.y;//y1 - self->boxMin.y;
                    //int z = 1;// + self->boxMin.z;//z1 - self->boxMin.z;
                    //int i = x + (y * self->size.x) + (z * self->size.x * self->size.y);

                    float3 pos0 = float3(x, y, z) * radiosityProbeScale + self->boxMin;
                    self->probes[x][y][z].canSeeSky = !worldRaycast(pos0, pos0 - sunDirection * 99);

                    for (int xx = (-offset + x); xx < (offset + 1 + x); xx++)
                    {
                        if (xx < 0 || xx >= self->count.x)
                            continue;

                        for (int yy = (-offset + y); yy < (offset + 1 + y); yy++)
                        {
                            if (yy < 0 || yy >= self->count.y)
                                continue;

                            for (int zz = (-offset + z); zz < (offset + 1 + z); zz++)
                            {
                                if (zz < 0 || zz >= self->count.z)
                                    continue;

                                float3 pos1 = float3(xx, yy, zz) * radiosityProbeScale + self->boxMin;

                                bool hitFound = worldRaycast(pos0, pos1);
                                float dist = distance(pos0, pos1);

                                if (hitFound)
                                {
                                    self->probes[x][y][z].occlusion[xx][yy][zz] = 999.0f;
                                }
                                else
                                {
                                    self->probes[x][y][z].occlusion[xx][yy][zz] = dist;
                                }
                            }
                        }
                    }

                }
            }
        }
    }


    /*
    int i = 123;
    DrawSphere(self->radiosityProbeLocation[i], 0.5);
    if (DrawButton("Radiosity"))
    {
        for (int i = 0; i < self->count; i++)
        {
            for (int j = 0; j < self->count; j++)
            {
                float3 pos0 = self->radiosityProbeLocation[i];
                float3 pos1 = self->radiosityProbeLocation[j];

                bool hitFound = false;
                for (int k = 0; k < ArrayCount(haven->entities); k++)
                {
                    if (haven->entities[k]->type == "StaticMesh")
                    {
                        StaticMesh* mesh = (StaticMesh*)haven->entities[k];
                        Transform t = GetLocalBoundsTransform(mesh);
                        float3 startLocal = WorldToLocal(t, pos0);
                        float3 endLocal = WorldToLocal(t, pos1);
                        if (RayBoxIntersect(startLocal, endLocal))
                        {
                            hitFound = true;
                            break;
                        }
                    }
                }

                int q = i + j * self->count;
                float dist = distance(pos0, pos1);
                if (hitFound)
                    dist = 0;

                self->radiosityProbeOcclusion[q] = dist;

                self->radiosityProbeColors[j] = float3(dist, dist, dist) * 0.1;
                if (!hitFound)
                    self->radiosityProbeColors[j] = float3(1, 0, 0);
            }
        }
    }
    */

}
