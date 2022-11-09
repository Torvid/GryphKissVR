    
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

typedef void EntityStart(EngineState* engineState, Entity* entity, Input* input);
typedef void EntityUpdate(EngineState* engineState, Entity* entity, Input* input);

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

#define Instantiate(engineState, type) (type*)(engineState->arenaScene.base + engineState->arenaScene.used); \
    ArenaPushStruct(&engineState->arenaScene, type, "");

#define Instantiate(engineState, type, transform) (type*)(engineState->arenaScene.base + engineState->arenaScene.used); \
    type* t = ArenaPushStruct(&engineState->arenaScene, type, "") \
    t->transform = transform;

//void Main(EngineState* engineState)
//{
//    EntityLightDirectional* a = Instantiate(engineState, EntityLightDirectional);
//}

struct Assets
{
#define assetRegistryLoad(function, type, name, filename) type* name;
#include "assetRegistry.cpp"
#undef assetRegistryLoad
};

static Assets* assets;

struct EngineState
{
    PlatformPrint* platformPrint;
    Printf* printf;
    sPrintf* sprintf;
    Memset* memset;
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
    //ArrayCreate(Mesh, stringMeshes, 100);
    ArrayCreate(Animation, animations, 100);
    ArrayCreate(RenderCommand, renderCommands, 5000);

    ArrayCreate(Entity*, entities, 1000); // pointers to all entities in the scene

    Shader* missingShader;
    Shader* unlit;
    Shader* defaultlit;
    Shader* UIShader;
    Shader* postProcessShader;
    Shader* textureCopyShader;
    Shader* rippleSimShader;

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
static EngineState* globalEngineState;
static GameMemory* globalGameMemory;

#include "loadMesh.cpp"
#include "loadAnimation.cpp"
#include "loadShader.cpp"
#include "loadSound.cpp"
#include "loadTexture.cpp"



#define Cpp 1

void LoadAssets(EngineState* engineState, Assets* assets)
{
#define assetRegistryLoad(function, type, name, filename) assets->name = function(engineState, filename);
#include "assetRegistry.cpp"
#undef assetRegistryLoad
}

#include "defaultlit.shader"
#include "missing.shader"
#include "PostProcessTest.shader"
#include "rippleSim.shader"
#include "textureCopy.shader"
#include "UI.shader"
#include "unlit.shader"

typedef void SetupShaderFunction(Shader* shader);

SetupShaderFunction* ShaderSetupFunctions[] = {
    SetupShader_defaultlit,
    SetupShader_missing,
    SetupShader_PostProcessTest,
    SetupShader_rippleSim,
    SetupShader_textureCopy,
    SetupShader_UI,
    SetupShader_unlit,
};

char* ShaderNames[] = {
    "defaultlit.shader",
    "missing.shader",
    "PostProcessTest.shader",
    "rippleSim.shader",
    "textureCopy.shader",
    "UI.shader",
    "unlit.shader"
};

#define CreateMaterialGlobal(engineState, name, _shader, type) \
    name = (type*)ArenaPushStruct(&engineState->arenaGlobalDrawCommands, type, ""); \
    name->shader = (_shader);\
    name->BackFaceCulling = true;\
    name->Wireframe = false;\
    SetShaderDefaults(name);\

// mouseCursorCommand2, engineState->UIShader, Material_UI
#define CreateMaterialLocal(name, _shader, type) \
    type name##Data = {};\
    type* name = &name##Data;\
    name->shader = (_shader);\
    name->transform = Transform(vectorZero, vectorForward, vectorUp, vectorOne);\
    name->BackFaceCulling = true;\
    name->Wireframe = false;\
    SetShaderDefaults(name);\


void DrawClear(EngineState* memory, float3 color = {0,0,0})
{
    RenderCommand* result = ArrayAddNew(memory->renderCommands);
    result->type = RenderCommand_Clear;
    result->clearColor = { color.x, color.y, color.z, 1 };
    result->clearColorEnabled = true;
    result->clearDepthEnabled = true;
}

void DrawClearDepth(EngineState* memory)
{
    RenderCommand* result = ArrayAddNew(memory->renderCommands);
    result->type = RenderCommand_Clear;
    //result->clearColor = clearColor;
    result->clearColorEnabled = false;
    result->clearDepthEnabled = true;
}
void DrawClearColor(EngineState* memory, float3 color = { 0,0,0 })
{
    RenderCommand* result = ArrayAddNew(memory->renderCommands);
    result->type = RenderCommand_Clear;
    result->clearColor = { color.x, color.y, color.z, 1 };
    result->clearColorEnabled = true;
    result->clearDepthEnabled = false;
}

void SetRenderTarget(EngineState* memory, Texture* target)
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
    ArrayAdd(memory->renderCommands, result);
}

Texture* CreateFramebufferTarget(EngineState* engineState)
{
    Texture* result = ArrayAddNew(engineState->textures);
    result->isFramebufferTarget = true;
    engineState->platformGraphicsCreateFramebufferTarget(result);
    return result;
}

Texture* CreateTextureTarget(EngineState* engineState, int sizeX, int sizeY)
{
    Texture* result = ArrayAddNew(engineState->textures);
    result->isTextureTarget = true;
    result->width = sizeX;
    result->height = sizeY;
    engineState->platformGraphicsCreateTextureTarget(result);
    return result;
}

void printTransform(EngineState* engineState, Transform t2)
{
    engineState->printf("pos  : (%.2f, %.2f, %.2f)\n", t2.position.x, t2.position.y, t2.position.z);
    engineState->printf("right  : (%.2f, %.2f, %.2f)\nforward: (%.2f, %.2f, %.2f)\nup     : (%.2f, %.2f, %.2f)\n",
        t2.right.x, t2.right.y, t2.right.z,
        t2.forward.x, t2.forward.y, t2.forward.z,
        t2.up.x, t2.up.y, t2.up.z);
}

#include "ui.cpp"
#include "profiling.cpp"

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
    if (!globalEngineState)
        return;
    ProfilerBeingSample(globalEngineState);
}
void engineProfilerEndSample(char* name)
{
    if (!globalEngineState)
        return;
    ProfilerEndSample(globalEngineState, name);
}

extern "C" __declspec(dllexport) void gameUpdateAndRender(GameMemory* memory)
{
    globalMemset = memory->memset;
    memory->engineProfilerBeingSample = &engineProfilerBeingSample;
    memory->engineProfilerEndSample = &engineProfilerEndSample;

    EngineState* engineState = (EngineState*)(&(memory->memory));
    // Set global variables!
    globalEngineState = engineState;
    assets = &engineState->assets;

    engineState->timeStart = memory->platformTime();
    engineState->externalTime = ((engineState->timeStart - engineState->timeEnd) * 100) / 1000;
    if (memory->initialized)
    {
        ProfilerStart(engineState);
        ProfilerBeingSample(engineState);
    }

    engineState->Resolution = memory->Resolution;

    ArrayClear(engineState->renderCommands);
    ArenaReset(&engineState->arenaDrawCommands);
    Input* input = &memory->input;

    // hit space = smash game memory to 0 = restart game
    if (input->spaceDown)
    {
        memory->initialized = false;
        StructClear(engineState);
        //memory->memset(engineState, 0, sizeof(*engineState));
        //memory->reloadNow = true;
    }

    if (memory->reloadNow)
    {
        memory->reloadNow = false;
        ArenaReset(&engineState->arenaHotreload);
        engineState->coutner++;
        for (int i = 0; i < ArrayCount(engineState->meshes); i++)
        {
            if (engineState->meshes[i].filename[0] == 0)
                continue;
            if (EndsWith(&engineState->meshes[i].filename[0], ".mesh") || EndsWith(&engineState->meshes[i].filename[0], ".obj"))
                LoadMesh(engineState, &engineState->meshes[i]);
        }
        for (int i = 0; i < ArrayCount(engineState->meshes); i++)
        {
            if (engineState->animations[i].filename[0] == 0)
                continue;
            if (EndsWith(&engineState->animations[i].filename[0], ".anim"))
                LoadAnimation(engineState, &engineState->animations[i]);
        }
        for (int i = 0; i < ArrayCount(engineState->textures); i++)
        {
            if (engineState->textures[i].filename[0] == 0)
                continue;
            if (!EndsWith(&engineState->textures[i].filename[0], ".tga"))
                continue;
            LoadTexture(engineState, &engineState->textures[i]);
        }
        for (int i = 0; i < ArrayCount(engineState->shaders); i++)
        {
            if (engineState->shaders[i].filename[0] == 0)
                continue;
            if (!EndsWith(&engineState->shaders[i].filename[0], ".shader"))
                continue;
            LoadShader(engineState, &engineState->shaders[i]);
            ShaderSetupFunctions[i](&engineState->shaders[i]);
            (&engineState->shaders[i])->GLID = memory->platformGraphicsLoadShader(&engineState->shaders[i]);
        }
        for (int i = 0; i < ArrayCount(engineState->sounds); i++)
        {
            if (engineState->sounds[i].filename[0] == 0)
                continue;
            if (!EndsWith(&engineState->sounds[i].filename[0], ".wav"))
                continue;
            LoadWav(engineState, &engineState->sounds[i]);
        }
    }
    bool firstFrame = false;
    if (!memory->initialized)
    {
        firstFrame = true;
        engineState->coutner = 0;
        memory->initialized = true;

        ArenaInitialize(&engineState->arenasArena, sizeof(engineState->arenasArenaData), engineState->arenasArenaData);

        ArenaInitialize(&engineState->arenaHotreload,           Megabytes(512), (uint8*)ArenaPushBytes(&engineState->arenasArena, Megabytes(512),   "Hotreload",            true), "Hotreload");
        ArenaInitialize(&engineState->arenaDrawCommands,        Megabytes(64),  (uint8*)ArenaPushBytes(&engineState->arenasArena, Megabytes(64),    "Draw Commands",        true), "Draw Commands");
        ArenaInitialize(&engineState->arenaGlobalDrawCommands,  Megabytes(64),  (uint8*)ArenaPushBytes(&engineState->arenasArena, Megabytes(64),    "Global Draw Commands", true), "Global Draw Commands");
        ArenaInitialize(&engineState->arenaEngineState,         Megabytes(64),  (uint8*)ArenaPushBytes(&engineState->arenasArena, Megabytes(64),    "Engine State",         true), "Engine State");
        ArenaInitialize(&engineState->arenaScene,               Megabytes(64),  (uint8*)ArenaPushBytes(&engineState->arenasArena, Megabytes(64),    "Scene",                true), "Scene");
        

        engineState->profilingData = ArenaPushStruct(&engineState->arenaEngineState, ProfilingData, "Profiling Data");
        engineState->uiMeshData = ArenaPushStruct(&engineState->arenaEngineState, UIMeshData, "UI Mesh Data");

        for (int i = 0; i < ArrayCapacity(engineState->soundChannels); i++)
        {
            engineState->soundChannels[i] = ArenaPushStruct(&engineState->arenaEngineState, SoundChannel, "Sound Channel");
        }

        engineState->platformPrint = memory->platformPrint;
        engineState->printf = memory->printf;
        engineState->sprintf = memory->sprintf;
        engineState->memset = memory->memset;
        engineState->platformTime = memory->platformTime;
        engineState->platformReadFile = memory->platformReadFile;
        engineState->platformWriteFile = memory->platformWriteFile;
        engineState->platformGraphicsLoadTexture = memory->platformGraphicsLoadTexture;
        engineState->platformGraphicsLoadMesh = memory->platformGraphicsLoadMesh;
        engineState->platformGraphicsLoadShader = memory->platformGraphicsLoadShader;
        engineState->platformGraphicsCreateFramebufferTarget = memory->platformGraphicsCreateFramebufferTarget;
        engineState->platformGraphicsCreateTextureTarget = memory->platformGraphicsCreateTextureTarget;


        memory->renderCommands = engineState->renderCommands;

        Shader** shaders[] = {
            &engineState->defaultlit,
            &memory->missingShader,
            &engineState->postProcessShader,
            &engineState->rippleSimShader,
            &engineState->textureCopyShader,
            &engineState->UIShader,
            &engineState->unlit
        };

        for (size_t i = 0; i < ArrayCapacity(shaders); i++)
        {
            *shaders[i] = FileReadShader(engineState, ShaderNames[i]);
            ShaderSetupFunctions[i](*shaders[i]);
            (*shaders[i])->GLID = memory->platformGraphicsLoadShader(*shaders[i]);
        }
        LoadAssets(engineState, &engineState->assets);
        memory->missingTexture = assets->missing;
        memory->missingMesh = assets->missingMesh;


        CreateMaterialGlobal(engineState, engineState->axesMaterial, engineState->unlit,  Material_unlit);
        engineState->axesMaterial->ColorTexture = assets->texAxes;
        engineState->axesMaterial->Color = float3(1, 1, 1);
        engineState->axesMaterial->BackFaceCulling = true;

        CreateMaterialGlobal(engineState, engineState->boneMaterial, engineState->unlit, Material_unlit);
        engineState->boneMaterial->shader = engineState->unlit;
        engineState->boneMaterial->ColorTexture = assets->white;
        engineState->boneMaterial->Color = float3(38.0f / 255.0f, 0, 67.0f / 255.0f);

        engineState->boneMaterial->Wireframe = true;
        engineState->boneMaterial->BackFaceCulling = false;


        engineState->SwapBuffer = CreateFramebufferTarget(engineState);
        engineState->waterRipples0 = CreateTextureTarget(engineState, 16, 16);
        engineState->waterRipples1 = CreateTextureTarget(engineState, 16, 16);

        engineState->stringMesh = ArrayAddNew(engineState->meshes);

        engineState->spectatorCamera = Transform(float3(-2, -2, 0), float3(0.5, 0.5, -0.5), vectorUp, vectorOne);


        soundStart(engineState, input, memory);

        editorStart(engineState, input);
        
        gryphkissStart(engineState, input);

        profilerStart(engineState, input);


        CreateMaterialGlobal(engineState, engineState->red, engineState->unlit, Material_unlit);
        engineState->red->ColorTexture = assets->white;
        engineState->red->Color = float3(1, 0, 0);
        engineState->red->BackFaceCulling = true;

        CreateMaterialGlobal(engineState, engineState->green, engineState->unlit, Material_unlit);
        engineState->green->ColorTexture = assets->white;
        engineState->green->Color = float3(0, 1, 0);
        engineState->green->BackFaceCulling = true;

        CreateMaterialGlobal(engineState, engineState->blue, engineState->unlit, Material_unlit);
        engineState->blue->ColorTexture = assets->white;
        engineState->blue->Color = float3(0, 0, 1);
        engineState->blue->BackFaceCulling = true;

        return;
    }


    soundUpdate(engineState, input, memory);

    input->head = input->eyeLeft;
    input->head.position = float3(0, 0, 0);
    input->head.position += input->eyeLeft.position;
    input->head.position += input->eyeRight.position;
    input->head.position *= 0.5f;

    if (input->mouseLeftDown || input->faceButtonRight || firstFrame)
    {
        engineState->waterRipplesPrevious = engineState->waterRipples0;
        engineState->waterRipplesCurrent = engineState->waterRipples1;

        // setup
        SetRenderTarget(engineState, engineState->waterRipplesPrevious);
        DrawClear(engineState);

        CreateMaterialLocal(copyTexture, engineState->textureCopyShader, Material_textureCopy);
        copyTexture->mesh = assets->ui_quad;
        copyTexture->ColorTexture = assets->coal;
        DrawMesh(engineState, copyTexture);


        SetRenderTarget(engineState, engineState->waterRipplesCurrent);
        DrawClear(engineState);

        copyTexture->ColorTexture = assets->coal;
        DrawMesh(engineState, copyTexture);
    }

    // draw
    SetRenderTarget(engineState, engineState->waterRipplesCurrent);
    DrawClear(engineState);

    CreateMaterialLocal(rippleSim, engineState->rippleSimShader, Material_rippleSim);
    rippleSim->mesh = assets->ui_quad;
    rippleSim->TexPrevious = engineState->waterRipplesPrevious;
    rippleSim->mousePos = input->mousePos / engineState->Resolution;
    DrawMesh(engineState, rippleSim);

    // swap
    Texture* temp;
    temp = engineState->waterRipplesPrevious;
    engineState->waterRipplesPrevious = engineState->waterRipplesCurrent;
    engineState->waterRipplesCurrent = temp;

    SetRenderTarget(engineState, engineState->SwapBuffer);
    DrawClear(engineState, float3(0.5, 0, 0));



    float2 pos = float2(0, 0);
    engineState->uiPos = float2(0, 0);
    

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
    CreateMaterialLocal(waterPlane, engineState->unlit, Material_unlit);
    waterPlane->ColorTexture = engineState->waterRipplesCurrent;
    waterPlane->Color = float3(1.0f, 1.0f, 1.0f);
    waterPlane->mesh = assets->ui_quad;
    waterPlane->BackFaceCulling = true;
    waterPlane->transform = identity;
    DrawMesh(engineState, waterPlane, assets->ui_quad, transform(center));

    DrawText(engineState, "G: toggle editor");
    DrawText(engineState, "P: toggle profiling");
    editorUpdate(engineState, input);
    memory->spectatorCamera = engineState->spectatorCamera;

    profilerUpdate(engineState, input);
    gryphkissUpdate(engineState, input);

    if (input->faceButtonLeftDown || input->faceButtonRightDown || input->gDown)
    {
        engineState->editor = !engineState->editor;
    }
    if (input->faceButtonLeftDown || input->faceButtonRightDown || input->pDown)
    {
        engineState->profiling = !engineState->profiling;
    }



    // Mouse cursor
    CreateMaterialLocal(mouseCursorCommand, engineState->UIShader, Material_UI);
    mouseCursorCommand->transform = {};
    mouseCursorCommand->mesh = assets->ui_circle;
    mouseCursorCommand->BackFaceCulling = false;
    mouseCursorCommand->Wireframe = false;
    mouseCursorCommand->DisableDepthTest = true;

    // Draw UI
    engineState->textTransform = input->head;
    engineState->textTransform.position += engineState->textTransform.forward * 3;
    engineState->textTransform.position += engineState->textTransform.up * 0.25;
    engineState->textTransform.position += engineState->textTransform.right * -0.25;

    // Draw the UI
    UpdateMesh(engineState, engineState->stringMesh,
        engineState->uiMeshData->vertexes,
        engineState->uiMeshData->quadCount * 4, 
        engineState->uiMeshData->indexes, 
        engineState->uiMeshData->quadCount * 6);

    CreateMaterialLocal(command, engineState->UIShader, Material_UI);
    command->mesh = engineState->stringMesh;
    command->transform = transformIdentity;
    command->BackFaceCulling = false;
    //command->Wireframe = false;
    //command->DisableDepthTest = true;
    command->blendMode = BlendMode_Alpha;
    command->FontTexture = assets->font;

    command->VRCameraPosition = engineState->textTransform.position;
    command->VRCameraForward = engineState->textTransform.forward;
    command->VRCameraUp = engineState->textTransform.up;
    command->VRCameraRight = engineState->textTransform.right;

    command->SpectatorCameraPosition = engineState->spectatorCamera.position;
    command->SpectatorCameraForward = engineState->spectatorCamera.forward;
    command->SpectatorCameraUp = engineState->spectatorCamera.up;
    command->SpectatorCameraRight = engineState->spectatorCamera.right;

    // Clear depth so UI is drawn on top.
    DrawMesh(engineState, command, false);
    engineState->uiMeshData->quadCount = 0;

    ProfilerEndSample(engineState, "GryphKissVR");
    
    ProfilerEnd(engineState);
    engineState->timeEnd = engineState->platformTime();
    engineState->internalTime = ((engineState->timeEnd - engineState->timeStart) * 100) / 1000;

    SetRenderTarget(engineState, 0);
    DrawClear(engineState);

    CreateMaterialLocal(finalOutputCommand, engineState->postProcessShader, Material_PostProcessTest);
    finalOutputCommand->mesh = assets->ui_quad;

    finalOutputCommand->ColorTexture = engineState->SwapBuffer;
    finalOutputCommand->TexRipples = engineState->waterRipplesCurrent;

    DrawMesh(engineState, finalOutputCommand);

    memory->renderCommands_count = engineState->renderCommands_count;
}

#endif

