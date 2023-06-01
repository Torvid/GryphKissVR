#pragma once
#include "haven.cpp"
#include "haven_platform.h"
#include "string.cpp"

// Arrays

#define ArrayCount(name) name##_count

#define ArrayCreate(type, name, capacity) type name[capacity] = {}; int name##_count = 0;
bool IndexIsInArray(int index, int size)
{
    return (index >= 0) && (index < size);
}
#define IsInArray(name, index) IndexIsInArray(index, ArrayCapacity(name))

#define ArrayFull(name) (ArrayCount(name) >= ArrayCapacity(name))

#define ArrayAdd(name, var) &name[ArrayCount(name)]; \
{ \
    ArrayCount(name)++; \
    Assert(sizeof(name[0]) == sizeof(var), "Item is a different size than other items in the array"); \
    Assert(IsInArray(name, ArrayCount(name) - 1), "Array is out of capacity."); \
    name[ArrayCount(name)-1] = var; \
}

#define ArrayAddNew(name) &name[ArrayCount(name)]; \
{ \
    ArrayCount(name)++; \
    Assert(IsInArray(name, ArrayCount(name) - 1), "Array is out of capacity."); \
    name[ArrayCount(name)-1] = {}; \
}

// scan backwards, moving items to the right by one, then insert.
#define ArrayInsert(name, var, index) \
{ \
    ArrayCount(name)++; \
    for (int _itter = ArrayCount(name); _itter >= index; _itter--) \
    { \
        name[_itter] = name[_itter-1]; \
    } \
    name[index] = var; \
} \

// loop over the array, swapping each element with a random one
#define ArrayShuffle(name) \
{ \
    for (int _itter = ArrayCount(name); _itter >= index; _itter--) \
    { \
        int _rand_index = Random(0, ArrayCount(name)-1); \
        name[ArrayCount(name)] = name[_itter]; \
        name[_itter] = name[_rand_index]; \
        name[_rand_index] = name[ArrayCount(name)]; \
    } \
} \

#define ArrayAddIndex(name) (&(name[ArrayCount(name)])); \
{ \
    ArrayCount(name)++; \
    Assert(IsInArray(name, ArrayCount(name) - 1), "Array is out of capacity."); \
} \

#define ArrayGetLast(name) (&(name[ArrayCount(name)-1]));

// Take the last item and swap paste it over the one we want to remove.
#define ArrayRemove(name, index) \
{ \
    Assert(IsInArray(name, index)); \
    ArrayCount(name)--; \
    name[index] = name[ArrayCount(name)]; \
} \

#define ArrayClear(name) ArrayCount(name) = 0;

#define ArrayAddRange(name, other) \
{ \
    int RemainingCapacity = ArrayCapacity(name) - ArrayCount(name); \
    Assert(RemainingCapacity > ArrayCount(other), "Array does not fit."); \
    Assert(sizeof(name[0]) == sizeof(other[0]), "Arrays have different item sizes."); \
    for (int _itter = 0; _itter < ArrayCount(other); _itter++) \
    { \
        ArrayAdd(name, other[_itter]); \
    } \
} \

#define ArrayFilledPercentage(name) (((float)ArrayCount(name)) / ((float)ArrayCapacity(name)))

void array_test()
{
    ArrayCreate(int, test, 32)
    ArrayCreate(int, test2, 16)
    ArrayAdd(test2, 100);
    ArrayAdd(test2, 101);
    ArrayAdd(test2, 102);
    ArrayAdd(test2, 103);
    ArrayAdd(test2, 104);
    ArrayAdd(test2, 105);

    int length = ArrayCount(test);

    // all macros all the way
    ArrayAdd(test, 1);
    ArrayAdd(test, 2);
    ArrayAdd(test, 3);
    ArrayAdd(test, 4);

    ArrayRemove(test, 0);
    ArrayAdd(test, 5);

    ArrayAddRange(test, test2);

    for (int i = 0; i < ArrayCount(test); i++)
    {
        test[i] *= 2;
    }
    ArrayCreate(int, my_array, 100);

    ArrayAdd(my_array, 1);
    ArrayAdd(my_array, 2);
    ArrayAdd(my_array, 1337);
    ArrayAdd(my_array, 9);

    int thing = my_array[2]; // 1337

    int count = ArrayCount(my_array); // 4

    bool full = ArrayFull(my_array); // false

    // add a new element without setting it, and return a pointer to it.
    int* newElement = ArrayAddNew(my_array);

    ArrayClear(my_array);

    count = ArrayCount(my_array); // 0

}



// Utilities

// Checks if two blocks of memory are the same
bool Equals(uint8* a, uint8* b, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

// Copies memory from one location to another
void Copy(uint8* source, uint8* destination, int size)
{
    globalMemcpy(destination, source, size);
    //for (int i = 0; i < size; i++)
    //{
    //    destination[i] = source[i];
    //}
}

// Sets a block of memory to 0
void Clear(uint8* destination, int size)
{
    globalMemset(destination, 0, size);
    //for (int i = 0; i < size; i++)
    //{
    //    destination[i] = 0;
    //}
}

// Copies a struct somewhere in memory
#define StructCopy(myStruct, destination) Copy(myStruct, destination, sizeof(myStruct))

// Clears a struct to 0
#define StructClear(myStruct) Clear((uint8*)myStruct, sizeof(*myStruct))



// Memory arena

struct MemoryArenaEntry
{
    float percentage;
    uint32 size;
    uint8* entry;
    char name[100];
};

struct MemoryArena
{
    float usedPercentage;
    uint32 size;
    uint8* base;
    uint32 used;
    ArrayCreate(MemoryArenaEntry, memoryArenaEntrys, 20000); // max 20000 things?
    char name[100];
};

void ArenaInitialize(MemoryArena* arena, uint32 Size, uint8* Base, const char* name = 0)
{
    if (name)
        StringCopy(arena->name, name);
    arena->size = Size;
    arena->base = Base;
    arena->used = 0;
    ArrayClear(arena->memoryArenaEntrys);
}

void ArenaReset(MemoryArena* arena)
{
    arena->used = 0;
    ArrayClear(arena->memoryArenaEntrys);
}

#define ArenaPushStruct(arena, type, name) (type*)ArenaPushBytes(arena, sizeof(type), name) // pushes 1x of a struct and returns pointer to start of it.
#define ArenaPushArray(arena, Count, type, name) (type*)ArenaPushBytes(arena, (Count)*sizeof(type), name) // pushes Nx of a struct and returns pointer to start of it.
void* ArenaPushBytes(MemoryArena* arena, uint32 size, const char* name = 0, bool clear = false)
{
    Assert(arena->base); // Arena is not initialized

    Assert((arena->used + (size)) <= arena->size); // Arena ran out of memory

    uint8* result = arena->base + arena->used;
    if(clear)
        Clear(result, size);

    arena->used += (size);
    arena->usedPercentage = ((float)arena->used / (float)arena->size);

    MemoryArenaEntry* entry = ArrayAddNew(arena->memoryArenaEntrys);
    entry->size = size;
    entry->percentage = ((float)size / (float)arena->size);
    if (name)
    {
        StringCopy(entry->name, name);
    }
    entry->entry = result;

    return result;
}
