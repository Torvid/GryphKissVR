#pragma once
#include "haven.cpp"

struct ANIMATION_HEADER
{
    char magicCookie[16];
    uint32 boneCount;
    uint32 frameCount;
    Transform data;
};

void LoadAnimation(EngineState* engineState, Animation* animation)
{
    // Push file into the hoterload arena
    ANIMATION_HEADER* header = (ANIMATION_HEADER*)(engineState->arenaHotreload.base + engineState->arenaHotreload.used);
    uint8* end = (uint8*)engineState->platformReadFile((uint8*)header, animation->filename);
    ArenaPushBytes(&engineState->arenaHotreload, (uint8*)end - (uint8*)header, animation->filename);

    // Crash if we read the wrong file
    Assert(Equals((uint8*)header->magicCookie, (uint8*)"h a v e n - anim", 16));

    // get metadata
    animation->boneCount = header->boneCount;
    animation->frameCount = header->frameCount;
    animation->transforms = &header->data;
    animation->length = header->frameCount / 30.0f;
}

Animation* FileReadAnimation(EngineState* engineState, const char* filename)
{
    ProfilerBeingSample(engineState);
    Animation* animation = ArrayAddNew(engineState->animations);
    StringCopy(animation->filename, filename);

    LoadAnimation(engineState, animation);

    ProfilerEndSample(engineState, "LoadAnimation");
    return animation;
}
