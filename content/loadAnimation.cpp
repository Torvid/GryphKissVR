#pragma once
#include "haven.cpp"

struct ANIMATION_HEADER
{
    char magicCookie[16];
    uint32 boneCount;
    uint32 frameCount;
    Transform data;
};

void LoadAnimation(Animation* animation)
{
    // Push file into the hoterload arena
    ANIMATION_HEADER* header = (ANIMATION_HEADER*)(haven->arenaAssets.base + haven->arenaAssets.used);
    uint8* end = (uint8*)haven->platformReadFile((uint8*)header, animation->filename);
    ArenaPushBytes(&haven->arenaAssets, (uint8*)end - (uint8*)header, animation->filename);

    // Crash if we read the wrong file
    Assert(Equals((uint8*)header->magicCookie, (uint8*)"h a v e n - anim", 16));

    // get metadata
    animation->boneCount = header->boneCount;
    animation->frameCount = header->frameCount;
    animation->transforms = &header->data;
    animation->length = header->frameCount / 30.0f;
}

Animation* FileReadAnimation(const char* filename)
{
    ProfilerBeingSample();
    Animation* animation = ArrayAddNew(haven->animations);
    StringCopy(animation->filename, filename);

    LoadAnimation(animation);

    ProfilerEndSample("LoadAnimation");
    return animation;
}
