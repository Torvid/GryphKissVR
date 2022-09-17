#pragma once
#include "haven_platform.h"
#include "memory.cpp"
//#include "memory.cpp"


void DrawArena(EngineState* engineState, float2* pos,  MemoryArena* arena, Transform textTransform)
{
    
    const int tempStringSize = 2500;
    char text[tempStringSize] = {};
    Clear((uint8*)text, tempStringSize);

    StringAppend(text, arena->name, " Used: ", (int)arena->used / 1024 / 1024);
    StringAppend(text, "/", (int)arena->size / 1024 / 1024);
    StringAppend(text, " MB - ", (int)arena->usedPercentage, "%");
    bool checkedEntries[ArrayCapacity(arena->memoryArenaEntrys)] = {};

    DrawText(engineState, text, pos, textTransform);
    Clear((uint8*)text, tempStringSize);

    for (int i = 0; i < ArrayCount(arena->memoryArenaEntrys); i++)
    {
        //Clear((uint8*)text, tempStringSize);

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
            DrawText(engineState, text, pos, textTransform);
            Clear((uint8*)text, tempStringSize);
        }
    }
    DrawText(engineState, text, pos, textTransform);
}
