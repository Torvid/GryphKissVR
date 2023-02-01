#pragma once
#include "haven_platform.h"
//#include "memory.cpp"
#include "haven.cpp"
//#include "math.cpp"
#include "ui.cpp"

static const int timeSampleCount = 2;

struct ProfilingData
{
    char functionNames[1000][50]; // 100 strings, 50 long
    int64 enterTime[1000][timeSampleCount];
    int64 exitTime[1000][timeSampleCount];
    int depth[1000];

    int64 timeStack[100][timeSampleCount];

    int frameNumber;
    int currentDepth;
    int index;
    //GameMemory* memory;
    //bool threading;
    float zoom = 0;

    int64 Global_Start;
    int64 Global_End;
    int64 Delta;

    //int64 outsideStart;
    //int64 outsideEnd;

    float deltas[30];
    int deltasIndex;

    float FPS[30];
    int FPSIndex;

    float deltas2[200];
    int deltas2Index;

};

void DrawArena(float2* pos,  MemoryArena* arena)
{
    const int tempStringSize = 2500;
    char text[tempStringSize] = {};
    Clear((uint8*)text, tempStringSize);
    
    //StringAppend(text, arena->name, " Used: ", (int)(arena->used * 100.0f) / 1024 / 1024);
    //float u = ((int)arena->used * 100.0f) / 1024 / 1024;
    //StringAppend(text, arena->name, " Used: ", u);
    StringAppend(text, "/", (int)arena->size / 1024 / 1024);
    StringAppend(text, " MB - ", (int)(arena->usedPercentage*100.0f), "%");
    bool checkedEntries[ArrayCapacity(arena->memoryArenaEntrys)] = {};

    DrawText(text, pos);
    Clear((uint8*)text, tempStringSize);

    for (int i = 0; i < ArrayCount(arena->memoryArenaEntrys); i++)
    {
        MemoryArenaEntry* entry = 0;
        int smallestSize = 0;
        int q = -1;
        // Find the biggest entry that has not been checked already
        for (int j = 0; j < ArrayCount(arena->memoryArenaEntrys); j++)
        {
            MemoryArenaEntry* entryj = &arena->memoryArenaEntrys[j];

            if (checkedEntries[j])
                continue;

            if (entryj->size >= smallestSize)
            {
                smallestSize = entryj->size;
                entry = entryj;
                q = j;
            }
        }

        checkedEntries[q] = true;
        if (entry == 0)
            continue;

        int kbSize = (int)((entry->size) / 1024);
        if (kbSize < 5)
            continue;

        StringAppend(text, "    ", kbSize, "KB, ");
        StringAppend(text, entry->name);
        StringAppend(text, "\n");
        if (StringLength(text) > (tempStringSize - 500))
        {
            DrawText(text, pos);
            Clear((uint8*)text, tempStringSize);
        }
    }
    DrawText(text, pos);
}
void DrawArena(MemoryArena* arena)
{
    DrawArena(&haven->uiPos, arena);
}
// Call this to start a profile region
void ProfilerBeingSample()
{
    ProfilingData* data = haven->profilingData;

    data->timeStack[data->currentDepth][data->frameNumber] = haven->platformTime();
    data->currentDepth++;
}

// Call this to end a profile region
void ProfilerEndSample(const char* name)
{
    ProfilingData* data = haven->profilingData;

    data->enterTime[data->index][data->frameNumber] = data->timeStack[data->currentDepth - 1][data->frameNumber];
    data->exitTime[data->index][data->frameNumber] = haven->platformTime();
    for (int i = 0; i < 50; i++)
    {
        data->functionNames[data->index][i] = name[i];
    }
    data->depth[data->index] = data->currentDepth;
    data->currentDepth--;
    data->index++;
}

// Call this somewhere near the start of the frame
void ProfilerStart()
{
    ProfilerEndSample(" h a v e n - Platform Layer");
    //ProfilerEndSample("Frame");
    ProfilingData* data = haven->profilingData;
    //data->memory = memory;
    //data->Global_End = data->Global_Start;
    int64 lastStart = data->Global_Start;
    data->Global_Start = haven->platformTime();
    data->Delta = (lastStart - data->Global_Start);
    if (data->Delta < 0)
        data->Delta *= -1;
    //data->outsideEnd = haven->platformTime();


}

// Call this somewhere near the end of the frame
void ProfilerEnd(EngineState* haven)
{
    ProfilingData* data = haven->profilingData;

    //ProfilerEndSample("Frame");




    //data->currentDepth--;
    data->index = 0;
    data->currentDepth = 0;
    data->frameNumber++;
    data->frameNumber %= timeSampleCount;
    data->Global_End = haven->platformTime();
    data->Delta = data->Global_End - data->Global_Start;
    //data->outsideStart = haven->platformTime();
    //ProfilerBeingSample(engineState);

    //data->index++;
    ProfilerBeingSample();
}


void ProfilerDrawFlameChart(ProfilingData* data)
{
    ProfilingData* profilingData = data;

    int64 lowest = 0;
    int64 highest = 0;
    for (int k = 0; k < timeSampleCount; k++)
    {
        lowest += profilingData->enterTime[profilingData->index - 1][k];
        highest += profilingData->exitTime[profilingData->index - 1][k];
    }
    lowest /= timeSampleCount;
    highest /= timeSampleCount;
    float rightOffset = haven->Resolution.x / 2;
    //float rightOffset = 720;
    
    float pixelWidth = haven->Resolution.x / (haven->Resolution.x - rightOffset);
    // Zoom
    if (input->mousePos.x > rightOffset && input->mousePos.y > (haven->Resolution.y - 80))
    {
        profilingData->zoom -= profilingData->zoom * input->mouseScrollDelta * 0.1f;
        profilingData->zoom = max(profilingData->zoom, 0.1f);
    }
    profilingData->zoom = 1.0f;

    int64 length = highest - lowest;
    float sampleCount = (300000 * profilingData->zoom) * pixelWidth + 15000;
    float milliseconds = (float)length / 10000.0f;
    float StepSize = 10000.0f / (float)length;


    float q = round(((float)sampleCount) / (100000)) * 0.5f;
    
    float flipOffset = 80;

    DrawBox2D(float2(rightOffset, flipOffset - 80), float2(haven->Resolution.x - rightOffset, 80), float3(0.0, 0.0, 0.0), 0.5);
    
    DrawLine2D(float2(rightOffset, flipOffset), float2(haven->Resolution.x, flipOffset), 1);
    //DrawLine2D(videoBuffer, float2(rightOffset, videoBuffer->Height - 1), float2(videoBuffer->Width, videoBuffer->Height - 1));
    for (int i = 0; i < 21; i++)
    {
        int point = (((float)i * 10000 * q) / ((float)sampleCount)) * haven->Resolution.x + rightOffset;
        DrawLine2D(float2(point,     flipOffset), float2(point,     flipOffset - 8), 1);
        DrawLine2D(float2(point + 1, flipOffset), float2(point + 1, flipOffset - 8), 1);
        //DrawLine2D(videoBuffer, float2(point, videoBuffer->Height - 0), float2(point, videoBuffer->Height - 8));
        //DrawLine2D(videoBuffer, float2(point + 1, videoBuffer->Height - 0), float2(point + 1, videoBuffer->Height - 8));
        char time[100] = {};
        StringAppend(time, (int)(i * q));
        StringAppend(time, "ms");
        DrawText(time, float2(point + 2, flipOffset - 20), float3(1, 1, 1));
    }
    float pos = 0;

    for (int i = 0; i < profilingData->index; i++)
    {
        int64 start = 0;
        int64 end = 0;
        for (int k = 0; k < timeSampleCount; k++)
        {
            start += profilingData->enterTime[i][k] - lowest;
            end += profilingData->exitTime[i][k] - lowest;
        }
        start /= timeSampleCount;
        end /= timeSampleCount;
        char* name = profilingData->functionNames[i];

        float startNormalized = (float)start / sampleCount;
        float endNormalized = (float)end / sampleCount;
        startNormalized *= haven->Resolution.x;
        endNormalized *= haven->Resolution.x;
        //startNormalized = max(startNormalized, 0);

        float height = flipOffset - (17 * profilingData->depth[i]);
        float2 pos = float2(round(startNormalized), height) + float2(rightOffset, 0);
        float2 size = float2(round(endNormalized - startNormalized) - 1, 16);
        DrawBox2D(pos, size, float3(0.5, 0.5, 0.5), 1.0f);
        char time[100] = {};
        StringAppend(time, name);
        StringAppend(time, ": ");
        StringAppend(time, (int)((end - start) / 10));
        StringAppend(time, "us");
        DrawText(time, float2(startNormalized + rightOffset + 2, height), float3(1,1,1));
        //DrawFont(haven->fontSpritesheet, videoBuffer, startNormalized + rightOffset + 2, height, time);
    }
}

void ProfilerDrawTimeChart(ProfilingData* data)
{
    ProfilingData* profilingData = data;

    DrawBox2D(float2(0, haven->Resolution.y - 200), float2(200, 200), float3(0.0, 0.0, 0.0), 0.5);

    float FPS = (float)(1.0f / input->deltaTime);
    float Delta = (float)(profilingData->Delta) / 10000000;
    float accumulatedDelta = GetSmoothedValue(profilingData->deltas, &profilingData->deltasIndex, Delta, 30);
    float SmoothFPS = GetSmoothedValue(profilingData->FPS, &profilingData->FPSIndex, FPS, 30);

    float TotalTimeDelta = (float)(profilingData->Delta) / 10000000;
    float SmoothedTime = GetSmoothedValue(profilingData->deltas2, &profilingData->deltas2Index, TotalTimeDelta, 100);

    float HeightMultiplier = 5000;
    float LastHeight = 0;
    for (int i = 0; i < 100; i++)
    {
        int h = (i + profilingData->deltas2Index) % 100;
        float a = profilingData->deltas2[h] * HeightMultiplier;
        float height = haven->Resolution.y - a;
        //DrawLine2D(videoBuffer, float2(i * 2 - 2, LastHeight), float2(i * 2, height));

        LastHeight = height;
    }


    float fpscurrentheight = haven->Resolution.y - (SmoothedTime)*HeightMultiplier;
    char text[100] = {};
    StringAppend(text, TotalTimeDelta * 1000);
    StringAppend(text, "ms");
    DrawText(text, float2(200, fpscurrentheight));
}

void profilerStart()
{

}

void profilerUpdate()
{
    // Update text
    const int tempStringSize = 2048;
    char text[tempStringSize] = {};
    Clear((uint8*)text, tempStringSize);

    if (haven->profiling)
    {
        //DrawArena(&pos, &haven->arenaDrawCommands);
        ProfilerDrawTimeChart(haven->profilingData);
        ProfilerDrawFlameChart(haven->profilingData);
        StringAppend(text, "\nPERFORMANCE:");
        StringAppend(text, "\n    Time: ", input->time);
        StringAppend(text, "\n    FPS: ", 1.0f / input->deltaTime);
        StringAppend(text, "\n    Internal Time: ", (int)haven->internalTime, " microseconds");
        StringAppend(text, "\n    External Time: ", (int)haven->externalTime, " microseconds");
        StringAppend(text, "\n    Total frame time: ", (int)(input->deltaTime * 1000), " milliseconds");
        DrawText(text);

        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "\n\nMEMORY:");
        DrawText(text);

        //DrawArena(&pos, &haven->arenaGlobalDrawCommands, textTransform);
        //DrawArena(&pos, &haven->arenaGameState, textTransform);
        DrawArena(&haven->arenaAssets);


        
        for (int i = 0; i < ArrayCapacity(haven->arenas); i++)
        {
            MemoryArena* arena = &haven->arenas[i];
            int w = arena->size / Megabytes(1);

            float2 pos = float2(300, 200 + 25 * i);
            DrawBox2D(pos, float2(w, 10), float3(1, 1, 1), 1);
            DrawBox2D(pos, float2(w * arena->usedPercentage, 10), float3(1, 0, 0), 1);
            //DrawText()
            DrawText((char*)haven->arenas[i].name, pos - float2(0, 14));
            //DrawText(haven->arenas[i].name, pos, float3(1, 1, 1), 1);
            //DrawText("what", pos, float3(1, 1, 1), 1);
            //arena->usedPercentage
        }
        
    }
}
