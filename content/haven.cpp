    
#pragma once
// Include guard
#ifndef havenIncluded
#define havenIncluded


/*
// functions needed for LLVM/CLANG/LLD to be happy :)
extern "C" __declspec(dllexport) bool _DllMainCRTStartup(void* hInstance, unsigned long dwReason, void* lpReserved)
{
	return true;
}
extern "C" __declspec(dllexport) void* memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char*)dest)[i] = ((char*)src)[i];
    }
	return dest;
}

extern "C" __declspec(dllexport) void* memset(void* dest, int ch, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((int*)dest)[i] = ch;
    }
    return dest;
}

// is this important?
extern "C" __declspec(dllexport) void __chkstk(){}
extern "C" { int _fltused = 0; }
*/

#include "haven_platform.h"

Memset* globalMemset;
Memcpy* globalMemcpy;

#include "math.cpp"
#include "memory.cpp"
#include "string.cpp"
#include "enumTrick.cpp"


struct GameState;
struct ProfilingData;
struct UIMeshData;
struct SoundChannel;
struct Material_unlit;

#define EntityTypeTable(n, X) \
    X(n, StaticMesh) \
    X(n, SkinnedMesh) \
    X(n, Light) \
    X(n, LightDirectional)
MakeEnum(EntityType, EntityTypeTable);

struct Entity;
struct EngineState;

typedef void EntityStart(Entity* entity, Input* input);
typedef void EntityUpdate(Entity* entity, Input* input);

#define EntityContents \
    EntityStart* entityStart; \
    EntityUpdate* entityUpdate; \
    bool alive; \
    Transform transform; \
    char* name[100]; \
    EntityType type; \
    bool visible; \
    bool hiddenInGame;

struct Entity
{
    EntityContents;
};

struct StaticMesh
{
    EntityContents;
    Mesh* mesh;
    Material* material;
};

struct SkinnedMesh
{
    EntityContents;
    Mesh* mesh;
    Material* material;
};

struct Light
{
    EntityContents;
    float3 color;
    float intensity;
    float radius;
};

struct LightDirectional
{
    EntityContents;
    float3 color;
    float intensity;
    float radius;
};

#define Instantiate(type) (type*)(haven->arenaScene.base + haven->arenaScene.used); \
    ArenaPushStruct(&haven->arenaScene, type, "");

#define Instantiate(type, transform) (type*)(haven->arenaScene.base + haven->arenaScene.used); \
    type* t = ArenaPushStruct(&haven->arenaScene, type, "") \
    t->transform = transform;

struct Assets
{
#define assetRegistryLoad(function, type, name, filename) type* name
#define assetRegistryLoadShader(function, type, name, filename) type* name
#include "assetRegistry.cpp"
#undef assetRegistryLoadShader
#undef assetRegistryLoad
};

static Assets* assets;

struct EngineState
{
    PlatformPrint* platformPrint;
    Printf* printf;
    sPrintf* sprintf;
    Memset* memset;
    Memcpy* memcpy;
    PlatformTime* platformTime;
    PlatformReadFile* platformReadFile;
    PlatformWriteFile* platformWriteFile;
    PlatformGraphicsLoadTexture* platformGraphicsLoadTexture;
    PlatformGraphicsLoadMesh* platformGraphicsLoadMesh;
    PlatformGraphicsLoadShader* platformGraphicsLoadShader;
    PlatformGraphicsCreateFramebufferTarget* platformGraphicsCreateFramebufferTarget;
    PlatformGraphicsCreateTextureTarget* platformGraphicsCreateTextureTarget;

    ArrayCreate(Texture, textures, 100);
    ArrayCreate(Mesh, meshes, 100);
    ArrayCreate(Shader, shaders, 100);
    ArrayCreate(Sound, sounds, 100);
    ArrayCreate(Animation, animations, 100);

    ArrayCreate(RenderCommand, renderCommands, 5000);
    ArrayCreate(Entity*, entities, 1000); // pointers to all entities in the scene

    Mesh* stringMesh;

    // ripple sim
    Texture* SwapBuffer;
    Texture* waterRipples0;
    Texture* waterRipples1;
    Texture* waterRipplesPrevious;
    Texture* waterRipplesCurrent;

    Material_unlit* axesMaterial;
    Material_unlit* boneMaterial;
    Material_unlit* red;
    Material_unlit* green;
    Material_unlit* blue;


    Transform spectatorCamera;

    SoundChannel* soundChannels[25];

    MemoryArena arenasArena;
    uint8 arenasArenaData[Megabytes(1024)];

    MemoryArena arenaHotreload; // arena for loaded assets like textures, meshes, etc. cleared on hoterload.
    MemoryArena arenaDrawCommands; // arena for draw caommands, cleared every frame
    MemoryArena arenaGlobalDrawCommands; // arena for draw commands, cleared every frame
    MemoryArena arenaEngineState; // memory arena for stuff of unknown size in the engine that persists forever.
    MemoryArena arenaScene; // memory arena for the scene, all live entities go here.

    uint8 scratchBuffer[Megabytes(256)];


    bool editor;
    bool profiling;

    int internalTime;
    int externalTime;
    int timeStart;
    int timeEnd;

    GameState* gameState;

    float2 Resolution;

    ProfilingData* profilingData;

    UIMeshData* uiMeshData;

    int coutner;
    float2 uiPos;

    Transform textTransform;
    
    Assets assets;
};
static EngineState* haven;
static GameMemory* globalGameMemory;

#include "ui.cpp"
#include "profiling.cpp"

#include "loadMesh.cpp"
#include "loadAnimation.cpp"
#include "loadShader.cpp"
#include "loadSound.cpp"
#include "loadTexture.cpp"

#define Cpp 1

#include "defaultlit.shader"
#include "missing.shader"
#include "postProcessTest.shader"
#include "rippleSimShader.shader"
#include "textureCopyShader.shader"
#include "UIShader.shader"
#include "unlit.shader"


void LoadAssets(Assets* assets)
{
#define assetRegistryLoad(function, type, name, filename) assets->name = function(filename)
#define assetRegistryLoadShader(function, type, name, filename) assets->name = function(filename); SetupShader_##name(assets->name); assets->name->GLID = haven->platformGraphicsLoadShader(assets->name)
#include "assetRegistry.cpp"
#undef assetRegistryLoadShader
#undef assetRegistryLoad
}

#define CreateMaterialGlobal(name, _shader, type) \
    name = (type*)ArenaPushStruct(&haven->arenaGlobalDrawCommands, type, ""); \
    name->shader = (_shader);\
    name->BackFaceCulling = true;\
    name->Wireframe = false;\
    SetShaderDefaults(name);\

// mouseCursorCommand2, haven->UIShader, Material_UI
#define CreateMaterialLocal(name, _shader, type) \
    type name##Data = {};\
    type* name = &name##Data;\
    name->shader = (_shader);\
    name->transform = Transform(vectorZero, vectorForward, vectorUp, vectorOne);\
    name->BackFaceCulling = true;\
    name->Wireframe = false;\
    SetShaderDefaults(name);\


void DrawClear(float3 color = {0,0,0})
{
    RenderCommand* result = ArrayAddNew(haven->renderCommands);
    result->type = RenderCommand_Clear;
    result->clearColor = { color.x, color.y, color.z, 1 };
    result->clearColorEnabled = true;
    result->clearDepthEnabled = true;
}

void DrawClearDepth()
{
    RenderCommand* result = ArrayAddNew(haven->renderCommands);
    result->type = RenderCommand_Clear;
    //result->clearColor = clearColor;
    result->clearColorEnabled = false;
    result->clearDepthEnabled = true;
}
void DrawClearColor(float3 color = { 0,0,0 })
{
    RenderCommand* result = ArrayAddNew(haven->renderCommands);
    result->type = RenderCommand_Clear;
    result->clearColor = { color.x, color.y, color.z, 1 };
    result->clearColorEnabled = true;
    result->clearDepthEnabled = false;
}

void SetRenderTarget(Texture* target)
{
    RenderCommand result = {};
    if (target == 0)
    {

    }
    else
    {
        // For it to be rendered to, it must be either a texture target or a framebuffer target.
        Assert(target->isFramebufferTarget || target->isTextureTarget);

        // It can't be both at the same time.
        if (target->isFramebufferTarget)
        {
            Assert(!target->isTextureTarget);
        }
        if (target->isTextureTarget)
        {
            Assert(!target->isFramebufferTarget);
        }
    }

    result.type = RenderCommand_SetRenderTarget;
    result.framebufferTarget = target;
    ArrayAdd(haven->renderCommands, result);
}

Texture* CreateFramebufferTarget(EngineState* engineState)
{
    Texture* result = ArrayAddNew(haven->textures);
    result->isFramebufferTarget = true;
    haven->platformGraphicsCreateFramebufferTarget(result);
    return result;
}

Texture* CreateTextureTarget(int sizeX, int sizeY)
{
    Texture* result = ArrayAddNew(haven->textures);
    result->isTextureTarget = true;
    result->width = sizeX;
    result->height = sizeY;
    haven->platformGraphicsCreateTextureTarget(result);
    return result;
}

void printTransform(Transform t2)
{
    haven->printf("pos  : (%.2f, %.2f, %.2f)\n", t2.position.x, t2.position.y, t2.position.z);
    haven->printf("right  : (%.2f, %.2f, %.2f)\nforward: (%.2f, %.2f, %.2f)\nup     : (%.2f, %.2f, %.2f)\n",
        t2.right.x, t2.right.y, t2.right.z,
        t2.forward.x, t2.forward.y, t2.forward.z,
        t2.up.x, t2.up.y, t2.up.z);
}


//Transform ApplyTransform(Transform t, Transform monkeyRotation)
//{
//    Transform t2 = {};
//    t2.scale = t.scale;
//    float3 d0 = float3(monkeyRotation.right.x, monkeyRotation.forward.x, monkeyRotation.up.x);
//    float3 d1 = float3(monkeyRotation.right.y, monkeyRotation.forward.y, monkeyRotation.up.y);
//    float3 d2 = float3(monkeyRotation.right.z, monkeyRotation.forward.z, monkeyRotation.up.z);
//    t2.position.x = dot(t.position, d0);
//    t2.position.y = dot(t.position, d1);
//    t2.position.z = dot(t.position, d2);
//    t2.position += monkeyRotation.position;
//
//    t2.right.x = -dot(d0, t.right);
//    t2.right.y = -dot(d1, t.right);
//    t2.right.z = -dot(d2, t.right);
//
//    t2.forward.x = dot(d0, t.forward);
//    t2.forward.y = dot(d1, t.forward);
//    t2.forward.z = dot(d2, t.forward);
//
//    t2.up.x = dot(d0, t.up);
//    t2.up.y = dot(d1, t.up);
//    t2.up.z = dot(d2, t.up);
//    return t2;
//}

#include "gryphkiss.cpp"
#include "editor.cpp"


void engineProfilerBeingSample()
{
    if (!haven)
        return;
    ProfilerBeingSample();
}
void engineProfilerEndSample(char* name)
{
    if (!haven)
        return;
    ProfilerEndSample(name);
}

extern "C" __declspec(dllexport) void gameUpdateAndRender(GameMemory* gameMemory)
{
    EngineState* engineState = (EngineState*)(&(gameMemory->memory));
    haven = engineState;
    globalGameMemory = gameMemory;

    globalMemset = gameMemory->memset;
    globalMemcpy = gameMemory->memcpy;
    gameMemory->engineProfilerBeingSample = &engineProfilerBeingSample;
    gameMemory->engineProfilerEndSample = &engineProfilerEndSample;

    // Set global variables!
    assets = &haven->assets;

    haven->timeStart = gameMemory->platformTime();
    haven->externalTime = ((haven->timeStart - haven->timeEnd) * 100) / 1000;
    if (gameMemory->initialized)
    {
        ProfilerStart();
        ProfilerBeingSample();
    }

    haven->Resolution = haven->Resolution;

    ArrayClear(haven->renderCommands);
    ArenaReset(&haven->arenaDrawCommands);
    Input* input = &gameMemory->input;

    // hit space = smash game memory to 0 = restart game
    if (input->spaceDown)
    {
        gameMemory->initialized = false;
        StructClear(haven);
    }
    
    if (gameMemory->reloadNow)
    {
        ProfilerBeingSample();
        gameMemory->reloadNow = false;
        ArenaReset(&haven->arenaHotreload);
        haven->coutner++;

        ArrayClear(haven->textures);
        ArrayClear(haven->meshes);
        ArrayClear(haven->shaders);
        ArrayClear(haven->sounds);
        ArrayClear(haven->animations);

        LoadAssets(&haven->assets);
        ProfilerEndSample("Hotreload");
    }

    bool firstFrame = false;
    if (!gameMemory->initialized)
    {
        firstFrame = true;
        haven->coutner = 0;
        gameMemory->initialized = true;

        ArenaInitialize(&haven->arenasArena, sizeof(haven->arenasArenaData), haven->arenasArenaData);

        ArenaInitialize(&haven->arenaHotreload,           Megabytes(512), (uint8*)ArenaPushBytes(&haven->arenasArena, Megabytes(512),   "Hotreload",            true), "Hotreload");
        ArenaInitialize(&haven->arenaDrawCommands,        Megabytes(64),  (uint8*)ArenaPushBytes(&haven->arenasArena, Megabytes(64),    "Draw Commands",        true), "Draw Commands");
        ArenaInitialize(&haven->arenaGlobalDrawCommands,  Megabytes(64),  (uint8*)ArenaPushBytes(&haven->arenasArena, Megabytes(64),    "Global Draw Commands", true), "Global Draw Commands");
        ArenaInitialize(&haven->arenaEngineState,         Megabytes(64),  (uint8*)ArenaPushBytes(&haven->arenasArena, Megabytes(64),    "Engine State",         true), "Engine State");
        ArenaInitialize(&haven->arenaScene,               Megabytes(64),  (uint8*)ArenaPushBytes(&haven->arenasArena, Megabytes(64),    "Scene",                true), "Scene");
        

        haven->profilingData = ArenaPushStruct(&haven->arenaEngineState, ProfilingData, "Profiling Data");
        haven->uiMeshData = ArenaPushStruct(&haven->arenaEngineState, UIMeshData, "UI Mesh Data");

        for (int i = 0; i < ArrayCapacity(haven->soundChannels); i++)
        {
            haven->soundChannels[i] = ArenaPushStruct(&haven->arenaEngineState, SoundChannel, "Sound Channel");
        }

        haven->platformPrint = gameMemory->platformPrint;
        haven->printf = gameMemory->printf;
        haven->sprintf = gameMemory->sprintf;
        haven->memset = gameMemory->memset;
        haven->platformTime = gameMemory->platformTime;
        haven->platformReadFile = gameMemory->platformReadFile;
        haven->platformWriteFile = gameMemory->platformWriteFile;
        haven->platformGraphicsLoadTexture = gameMemory->platformGraphicsLoadTexture;
        haven->platformGraphicsLoadMesh = gameMemory->platformGraphicsLoadMesh;
        haven->platformGraphicsLoadShader = gameMemory->platformGraphicsLoadShader;
        haven->platformGraphicsCreateFramebufferTarget = gameMemory->platformGraphicsCreateFramebufferTarget;
        haven->platformGraphicsCreateTextureTarget = gameMemory->platformGraphicsCreateTextureTarget;

        gameMemory->renderCommands = haven->renderCommands;

        LoadAssets(&haven->assets);
        gameMemory->missingTexture = assets->missingTexture;
        gameMemory->missingMesh = assets->missingMesh;


        CreateMaterialGlobal(haven->axesMaterial, assets->unlit,  Material_unlit);
        haven->axesMaterial->ColorTexture = assets->texAxes;
        haven->axesMaterial->Color = float3(1, 1, 1);
        haven->axesMaterial->BackFaceCulling = true;

        CreateMaterialGlobal(haven->boneMaterial, assets->unlit, Material_unlit);
        haven->boneMaterial->shader = assets->unlit;
        haven->boneMaterial->ColorTexture = assets->white;
        haven->boneMaterial->Color = float3(38.0f / 255.0f, 0, 67.0f / 255.0f);

        haven->boneMaterial->Wireframe = true;
        haven->boneMaterial->BackFaceCulling = false;


        haven->SwapBuffer = CreateFramebufferTarget(engineState);
        haven->waterRipples0 = CreateTextureTarget(16, 16);
        haven->waterRipples1 = CreateTextureTarget(16, 16);

        haven->stringMesh = ArrayAddNew(haven->meshes);

        haven->spectatorCamera = Transform(float3(-2, -2, 0), float3(0.5, 0.5, -0.5), vectorUp, vectorOne);


        soundStart(input, gameMemory);

        editorStart(input);
        
        gryphkissStart(input);

        profilerStart(input);


        CreateMaterialGlobal(haven->red, assets->unlit, Material_unlit);
        haven->red->ColorTexture = assets->white;
        haven->red->Color = float3(1, 0, 0);
        haven->red->BackFaceCulling = true;

        CreateMaterialGlobal(haven->green, assets->unlit, Material_unlit);
        haven->green->ColorTexture = assets->white;
        haven->green->Color = float3(0, 1, 0);
        haven->green->BackFaceCulling = true;

        CreateMaterialGlobal(haven->blue, assets->unlit, Material_unlit);
        haven->blue->ColorTexture = assets->white;
        haven->blue->Color = float3(0, 0, 1);
        haven->blue->BackFaceCulling = true;

        return;
    }


    soundUpdate(input, gameMemory);

    input->head = input->eyeLeft;
    input->head.position = float3(0, 0, 0);
    input->head.position += input->eyeLeft.position;
    input->head.position += input->eyeRight.position;
    input->head.position *= 0.5f;

    if (input->mouseLeftDown || input->faceButtonRight || firstFrame)
    {
        haven->waterRipplesPrevious = haven->waterRipples0;
        haven->waterRipplesCurrent = haven->waterRipples1;

        // setup
        SetRenderTarget(haven->waterRipplesPrevious);
        DrawClear();

        CreateMaterialLocal(copyTexture, assets->textureCopyShader, Material_textureCopyShader);
        copyTexture->mesh = assets->ui_quad;
        copyTexture->ColorTexture = assets->coal;
        DrawMesh(copyTexture);


        SetRenderTarget(haven->waterRipplesCurrent);
        DrawClear();

        copyTexture->ColorTexture = assets->coal;
        DrawMesh(copyTexture);
    }

    // draw
    SetRenderTarget(haven->waterRipplesCurrent);
    DrawClear();

    CreateMaterialLocal(rippleSim, assets->rippleSimShader, Material_rippleSimShader);
    rippleSim->mesh = assets->ui_quad;
    rippleSim->TexPrevious = haven->waterRipplesPrevious;
    rippleSim->mousePos = input->mousePos / haven->Resolution;
    DrawMesh(rippleSim);

    // swap
    Texture* temp;
    temp = haven->waterRipplesPrevious;
    haven->waterRipplesPrevious = haven->waterRipplesCurrent;
    haven->waterRipplesCurrent = temp;

    SetRenderTarget(haven->SwapBuffer);
    DrawClear(float3(0.5, 0, 0));



    float2 pos = float2(0, 0);
    haven->uiPos = float2(0, 0);
    

    input->vibrationAmplitudeRight = input->grabRight;


    input->handRight.scale = { 0.1f, 0.1f, 0.1f };
    input->handLeft.scale = { 0.1f, 0.1f, 0.1f };
    input->aimRight.scale = { 0.1f, 0.1f, 0.1f };
    input->aimLeft.scale = { 0.1f, 0.1f, 0.1f };

    float3 center = (input->playspaceStageLeft.position + input->playspaceStageRight.position) / 2.0f;
    Transform monkeyRotation = { center +
        float3(-1, 0, 0),
        input->playspaceStageLeft.right,
        input->playspaceStageLeft.forward,
        input->playspaceStageLeft.up,
        { 1, 1, 1 } };
    monkeyRotation = rotate(monkeyRotation, 0, 0, 0.0f);


    // Simulation plane
    Transform identity = Transform(vectorDown * 2.0, vectorForward, vectorUp, vectorOne);
    CreateMaterialLocal(waterPlane, assets->unlit, Material_unlit);
    waterPlane->ColorTexture = haven->waterRipplesCurrent;
    waterPlane->Color = float3(1.0f, 1.0f, 1.0f);
    waterPlane->mesh = assets->ui_quad;
    waterPlane->BackFaceCulling = true;
    waterPlane->transform = identity;
    DrawMesh(waterPlane, assets->ui_quad, transform(center));

    DrawText("G: toggle editor");
    DrawText("P: toggle profiling");
    editorUpdate(input);
    gameMemory->spectatorCamera = haven->spectatorCamera;

    profilerUpdate(input);
    gryphkissUpdate(input);

    if (input->faceButtonLeftDown || input->faceButtonRightDown || input->gDown)
    {
        haven->editor = !haven->editor;
    }
    if (input->faceButtonLeftDown || input->faceButtonRightDown || input->pDown)
    {
        haven->profiling = !haven->profiling;
    }



    // Mouse cursor
    CreateMaterialLocal(mouseCursorCommand, assets->UIShader, Material_UIShader);
    mouseCursorCommand->transform = {};
    mouseCursorCommand->mesh = assets->ui_circle;
    mouseCursorCommand->BackFaceCulling = false;
    mouseCursorCommand->Wireframe = false;
    mouseCursorCommand->DisableDepthTest = true;

    // Draw UI
    haven->textTransform = input->head;
    haven->textTransform.position += haven->textTransform.forward * 3;
    haven->textTransform.position += haven->textTransform.up * 0.25;
    haven->textTransform.position += haven->textTransform.right * -0.25;

    // Draw the UI
    UpdateMesh(haven->stringMesh,
        haven->uiMeshData->vertexes,
        haven->uiMeshData->quadCount * 4, 
        haven->uiMeshData->indexes, 
        haven->uiMeshData->quadCount * 6);

    CreateMaterialLocal(uiCommand, assets->UIShader, Material_UIShader);
    uiCommand->mesh = haven->stringMesh;
    uiCommand->transform = transformIdentity;
    uiCommand->BackFaceCulling = false;
    //command->Wireframe = false;
    //command->DisableDepthTest = true;
    uiCommand->blendMode = BlendMode_Alpha;
    uiCommand->FontTexture = assets->font;

    uiCommand->VRCameraPosition = haven->textTransform.position;
    uiCommand->VRCameraForward = haven->textTransform.forward;
    uiCommand->VRCameraUp = haven->textTransform.up;
    uiCommand->VRCameraRight = haven->textTransform.right;

    uiCommand->SpectatorCameraPosition = haven->spectatorCamera.position;
    uiCommand->SpectatorCameraForward = haven->spectatorCamera.forward;
    uiCommand->SpectatorCameraUp = haven->spectatorCamera.up;
    uiCommand->SpectatorCameraRight = haven->spectatorCamera.right;

    // Clear depth so UI is drawn on top.
    DrawMesh(uiCommand, false);
    haven->uiMeshData->quadCount = 0;

    ProfilerEndSample("GryphKissVR");
    
    ProfilerEnd(engineState);
    haven->timeEnd = haven->platformTime();
    haven->internalTime = ((haven->timeEnd - haven->timeStart) * 100) / 1000;

    SetRenderTarget(0);
    DrawClear();

    CreateMaterialLocal(finalOutputCommand, assets->postProcessTest, Material_postProcessTest);
    finalOutputCommand->mesh = assets->ui_quad;

    finalOutputCommand->ColorTexture = haven->SwapBuffer;
    finalOutputCommand->TexRipples = haven->waterRipplesCurrent;

    DrawMesh(finalOutputCommand);

    gameMemory->renderCommands_count = haven->renderCommands_count;
}

#endif

