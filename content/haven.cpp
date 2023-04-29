    
#pragma once
// Include guard
#ifndef havenIncluded
#define havenIncluded

#include "haven_platform.h"

Memset* globalMemset;
Memcpy* globalMemcpy;

#include "math.cpp"
#include "memory.cpp"w
#include "string.cpp"
#include "enumTrick.cpp"

const char* validchars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ$!?#&_%\'\",.;:^|`~=<>+-*/\\(){}[]@";

#pragma pack(push, 1)
struct Glyph
{
    uint16 minX;
    uint16 minY;
    uint16 maxX;
    uint16 maxY;
    uint16 pivotX;
    uint16 pivotY;
    uint16 advanceX;
    uint16 advanceY;
};
struct Font
{
    char cookie[16];
    Glyph glyphs[128];
    char filename[100];
};
#pragma pack(pop)


struct ProfilingData;
struct UIMeshData;
struct SoundChannel;
struct Material_unlit;
struct Material_defaultlit;

struct Entity;
struct EngineState;

struct Assets
{
#define assetRegistryLoad(function, type, name, filename) type* name
#define assetRegistryLoadShader(function, type, name, filename) type* name
#include "assetRegistry.cpp"
#undef assetRegistryLoadShader
#undef assetRegistryLoad
};

static Assets* assets;

enum HAlign
{
    HAlign_right,
    HAlign_center,
    HAlign_left,
};
enum VAlign
{
    VAlign_up,
    VAlign_center,
    VAlign_down,
};

struct EngineState
{
    PlatformPrint* platformPrint;
    Printf* printf;
    sPrintf* sprintf;
    PlatformTime* platformTime;
    PlatformReadFile* platformReadFile;
    PlatformWriteFile* platformWriteFile;
    PlatformGraphicsLoadTexture* platformGraphicsLoadTexture;
    PlatformGraphicsLoadMesh* platformGraphicsLoadMesh;
    PlatformGraphicsLoadShader* platformGraphicsLoadShader;
    PlatformGraphicsCreateFramebufferTarget* platformGraphicsCreateFramebufferTarget;
    PlatformGraphicsCreateTextureTarget* platformGraphicsCreateTextureTarget;
    PlatformGraphicsReadbackTexture* platformGraphicsReadbackTexture;
    PlatformGraphicsReadbackTextureHDR* platformGraphicsReadbackTextureHDR;

    ArrayCreate(Texture, globalTextures, 100);
    ArrayCreate(Texture, sceneTextures, 100);
    ArrayCreate(Mesh, meshes, 100);
    ArrayCreate(Shader, shaders, 100);
    ArrayCreate(Sound, sounds, 100);
    ArrayCreate(Animation, animations, 100);
    ArrayCreate(Font, fonts, 5);

    ArrayCreate(RenderCommand, renderCommands, 20000);
    ArrayCreate(Entity*, entities, 1000); // pointers to all entities in the scene

    ArrayCreate(Material_defaultlit*, sceneMaterials, 100);

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

    union
    {
        MemoryArena arenas[5];
        struct
        {
            MemoryArena arenaAssets; // arena for loaded assets like textures, meshes, etc. cleared on hoterload.
            MemoryArena arenaDrawCommands; // arena for draw caommands, cleared every frame
            MemoryArena arenaGlobalDrawCommands; // arena for draw commands, cleared every frame
            MemoryArena arenaEngineState; // memory arena for stuff of unknown size in the engine that persists forever.
            MemoryArena arenaScene; // memory arena for the scene, all live entities go here.
        };
    };

    uint8 scratchBuffer[Megabytes(256)];


    bool editor;
    bool profiling;

    int internalTime;
    int externalTime;
    int timeStart;
    int timeEnd;

    void* gameState;

    float2 Resolution;

    ProfilingData* profilingData;

    UIMeshData* uiMeshData;

    int coutner;
    float2 uiPos;

    Transform textTransform;
    
    Assets assets;
    HAlign hAlign;
    VAlign vAlign;
    float3 clearColor;
};
static EngineState* haven;
static Input* input;

static GameMemory* globalGameMemory;

#define EntityContents \
    bool alive; \
    Transform transform; \
    int type; \
    char* name[100]; \
    bool visible; \
    bool editorOnly;

struct Entity
{
    EntityContents;
};

#define Instantiate(_type) (_type*)InstantiateLocal(EntityType_##_type, sizeof(_type));
void* InstantiateLocal(int type, uint32 size)
{
    Entity* newEntity = (Entity*)ArenaPushBytes(&haven->arenaScene, size, "ent");// (haven->arenaScene.base + haven->arenaScene.used);
    newEntity->transform = transform(float3(0, 0, 0));
    newEntity->type = type;
    ArrayAdd(haven->entities, newEntity);
    return (void*)newEntity;
}

#include "ui.cpp"
#include "profiling.cpp"

#include "loadMesh.cpp"
#include "loadAnimation.cpp"
#include "loadShader.cpp"
#include "loadSound.cpp"
#include "loadTexture.cpp"
#include "loadFont.cpp"

#define Cpp 1

#include "defaultlit.shader"
#include "missing.shader"
#include "postProcessTest.shader"
#include "rippleSimShader.shader"
#include "textureCopyShader.shader"
#include "UIShader.shader"
#include "unlit.shader"
#include "reflectionProbeCubemapToOct.shader"
#include "compose.shader"
#include "downsize4x.shader"
#include "skydomeShader.shader"
#include "reflectionProbeToSphericalHarmonic.shader"
#include "reflectionProbeShader.shader"


void LoadAssets(Assets* assets)
{
    #define assetRegistryLoad(function, type, name, filename) assets->name = function(filename)
    #define assetRegistryLoadShader(function, type, name, filename) assets->name = function(filename); SetupShader_##name(assets->name); assets->name->GLID = haven->platformGraphicsLoadShader(assets->name)
    #include "assetRegistry.cpp"
    #undef assetRegistryLoadShader
    #undef assetRegistryLoad
}

#define CreateMaterialGlobal(name, _shader, type) \
    name = (PASTE(Material_, type)*)ArenaPushStruct(&haven->arenaGlobalDrawCommands, PASTE(Material_, type), ""); \
    name->shader = _shader; \
    name->BackFaceCulling = true; \
    name->Wireframe = false; \
    SetShaderDefaults(name);

#define CreateMaterialLocal(name, _shader, type) \
    PASTE(Material_, type) name##Data = {}; \
    PASTE(Material_, type)* name = &name##Data; \
    name->shader = _shader; \
    name->transform = Transform(vectorZero, vectorForward, vectorUp, vectorOne); \
    name->BackFaceCulling = true; \
    name->Wireframe = false; \
    name->mesh = assets->ui_quad; \
    SetShaderDefaults(name);

struct CustomRenderTexture
{
    Texture* current;
    Texture* previous;
};

enum ComposeTexturesMode
{
    ComposeTexturesMode_Add,
    ComposeTexturesMode_Subtract,
    ComposeTexturesMode_Multiply,
    ComposeTexturesMode_Divide,
    ComposeTexturesMode_Max,
    ComposeTexturesMode_Min,
    ComposeTexturesMode_IfNotZero,
    ComposeTexturesMode_Overwrite,
};

enum EntityType
{
    EntityType_Hand,
    EntityType_Player,
    EntityType_StaticMesh,
    EntityType_LightBaker,
    EntityType_ReflectionProbe,
    EntityType_Max,
};

#define structs 1
#include "entities/entities.cpp"
#undef structs

#define CreateSceneMaterial(name) \
    CreateMaterialGlobal(name, assets->defaultlit, defaultlit); \
    ArrayAdd(haven->sceneMaterials, name);

namespace Rendering
{
    void SetCubemap(Texture* texture)
    {
        for (int i = 0; i < ArrayCount(haven->sceneMaterials); i++)
        {
            haven->sceneMaterials[i]->texCubemap = texture;
        }
    }

    void SetLightmap(Texture* texture, float3 lightmapMin, float3 lightmapMax, float3 lightmapResolution, float radiosityProbeScale, float lightmapStrength = 0.005)
    {
        for (int i = 0; i < ArrayCount(haven->sceneMaterials); i++)
        {
            haven->sceneMaterials[i]->texLightmap = texture;
            haven->sceneMaterials[i]->lightmapMin = lightmapMin;
            haven->sceneMaterials[i]->lightmapMax = lightmapMax;
            haven->sceneMaterials[i]->lightmapResolution = lightmapResolution;
            haven->sceneMaterials[i]->radiosityProbeScale = radiosityProbeScale;
            haven->sceneMaterials[i]->lightmapStrength = lightmapStrength;
        }
    }

    void DrawClear(float3 color = {0,0,0}, const char* name = 0)
    {
        RenderCommand* result = ArrayAddNew(haven->renderCommands);
        result->type = RenderCommand_Clear;
        result->clearColor = { color.x, color.y, color.z, 1 };
        result->clearColorEnabled = true;
        result->clearDepthEnabled = true;
        result->name = name;
    }
    void DrawClearDepth(const char* name = 0)
    {
        RenderCommand* result = ArrayAddNew(haven->renderCommands);
        result->type = RenderCommand_Clear;
        //result->clearColor = clearColor;
        result->clearColorEnabled = false;
        result->clearDepthEnabled = true;
        result->name = name;
    }
    void DrawClearColor(float3 color = { 0,0,0 })
    {
        RenderCommand* result = ArrayAddNew(haven->renderCommands);
        result->type = RenderCommand_Clear;
        result->clearColor = { color.x, color.y, color.z, 1 };
        result->clearColorEnabled = true;
        result->clearDepthEnabled = false;
    }
    void SetRenderTarget(Texture* target, const char* name = 0)
    {
        RenderCommand* result = ArrayAddNew(haven->renderCommands);
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

        result->type = RenderCommand_SetRenderTarget;
        result->framebufferTarget = target;
        result->name = name;
    }

    Texture* CreateFramebufferTarget(EngineState* engineState, bool global = false)
    {
        Texture* result = 0;
        if (global)
        {
            result = ArrayAddNew(haven->globalTextures);
        }
        else
        {
            result = ArrayAddNew(haven->sceneTextures);
        }
        result->isFramebufferTarget = true;
        haven->platformGraphicsCreateFramebufferTarget(result);
        return result;
    }

    Texture* CreateTextureTarget(int sizeX, int sizeY, bool clamp = false, bool global = false)
    {
        Texture* result = 0;
        if (global)
        {
            result = ArrayAddNew(haven->globalTextures);
        }
        else
        {
            result = ArrayAddNew(haven->sceneTextures);
        }
        result->isTextureTarget = true;
        result->sizeX = sizeX;
        result->sizeY = sizeY;
        result->size = float2(sizeX, sizeY);
        result->aspectRatio = (float)sizeX / (float)sizeY;
        haven->platformGraphicsCreateTextureTarget(result, clamp);
        return result;
    }

    CustomRenderTexture* CreateCustomRenderTexture(int sizeX, int sizeY, bool clamp = false, bool global = false)
    {
        CustomRenderTexture* result = ArenaPushStruct(&haven->arenaEngineState, CustomRenderTexture, "CustomRenderTexture");
        result->current = Rendering::CreateTextureTarget(sizeX, sizeY, clamp, global);
        result->previous = Rendering::CreateTextureTarget(sizeX, sizeY, clamp, global);
        return result;
    }
    void Swap(CustomRenderTexture* texture)
    {
        Texture* temp = texture->current;
        texture->current = texture->previous;
        texture->previous = temp;
    }
    void DrawClear(CustomRenderTexture* texture)
    {
        SetRenderTarget(texture->current);
        DrawClear();
        SetRenderTarget(texture->previous);
        DrawClear();
        SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
    }
    void DrawClear(Texture* texture)
    {
        SetRenderTarget(texture);
        DrawClear();
        SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
    }

    void ComposeTextures(CustomRenderTexture* target, Texture* source, float2 pos, ComposeTexturesMode composeMode)
    {
        float2 targetSize = float2(target->current->sizeX, target->current->sizeY);
        float2 sourceSize = float2(source->sizeX, source->sizeY);

        Rendering::Swap(target);
        CreateMaterialLocal(comp, assets->compose, compose);
        comp->mesh = assets->ui_quad;
        comp->texA = target->previous;
        comp->texB = source;
        comp->Min = (pos) / targetSize;
        comp->Max = (pos + sourceSize) / targetSize;
        comp->composeMode = (int)composeMode;
        Rendering::SetRenderTarget(target->current);
        Rendering::DrawClear(float3(0, 0, 1));
        Rendering::DrawMesh(comp);
        Rendering::SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
    }
    void CubemapToSphericalHarmonic(Texture* target, Texture* source[6])
    {
        if (!target)
            return;

        for (int i = 0; i < 6; i++)
        {
            if (!source[i])
                return;
        }

        Rendering::SetRenderTarget(target, "Probe Capture");
        Rendering::DrawClear(float3(0, 1, 0));
        CreateMaterialLocal(octUnwrap, assets->reflectionProbeToSphericalHarmonic, reflectionProbeToSphericalHarmonic);
        octUnwrap->cubeTexture0 = source[0];
        octUnwrap->cubeTexture1 = source[1];
        octUnwrap->cubeTexture2 = source[2];
        octUnwrap->cubeTexture3 = source[3];
        octUnwrap->cubeTexture4 = source[4];
        octUnwrap->cubeTexture5 = source[5];
        octUnwrap->mesh = assets->ui_quad;
        Rendering::DrawMesh(octUnwrap);

        Rendering::SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
    }


    Camera PerspectiveCamera(Transform transform, float fov, float aspectRatio = 1.0, float maxDepth = 100.0f)
    {
        Camera camera;
        camera.enabled = true;
        camera.fov = fov;
        camera.maxDepth = maxDepth;
        camera.orthographic = false;
        camera.orthoWidth = 10;
        camera.transform = transform;
        //camera.transform.position = startPos + float3(-0.5, 0, -0.3);
        camera.aspectRatio = aspectRatio;
        return camera;
    }
    Camera OrthoCamera(Transform transform, float orthoWidth, float aspectRatio = 1.0, float maxDepth = 100.0f)
    {
        Camera camera;
        camera.enabled = true;
        camera.fov = 90;
        camera.maxDepth = maxDepth;
        camera.orthographic = true;
        camera.orthoWidth = orthoWidth;
        camera.transform = transform;
        //camera.transform.position = startPos + float3(-0.5, 0, -0.3);
        camera.aspectRatio = aspectRatio;
        return camera;
    }

    // assumes the texture target is set up already
    void DrawScene(Camera camera)
    {
        for (int j = 0; j < ArrayCount(haven->entities); j++)
        {
            Entity* entity = haven->entities[j];
            if (entity->type == EntityType_StaticMesh)
            {
                StaticMesh* mesh = (StaticMesh*)entity;
                if (StaticMeshCull(mesh, camera.transform))
                    continue;

                //bool last = mesh->material->BackFaceCulling;
                //mesh->material->BackFaceCulling = false;
                StaticMeshDraw(mesh, camera);
                //mesh->material->BackFaceCulling = last;
                //DrawMesh(mesh->material, mesh->mesh, mesh->transform, camera, "Scene StaticMesh");
            }
        }
    }

    void RenderOrtho(Transform transform, Texture* target, float orthoWidth, float aspectRatio, float maxDepth)
    {
        Rendering::SetRenderTarget(target, "Ortho Capture");
        Rendering::DrawClear();
        DrawScene(OrthoCamera(transform, orthoWidth, aspectRatio, maxDepth));
        Rendering::SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
    }

    void RenderCubemap(float3 startPos, Texture* cubeTexture[6])
    {
        for (int i = 0; i < 6; i++)
        {
            if (!cubeTexture[i])
                return;
        }
        //float2 crossLocation[6]
        //{
        //    float2(1, 1),
        //    float2(-1, 1),
        //    float2(2, 1),
        //    float2(0, 1),
        //    float2(0, 0),
        //    float2(1, 3),
        //};

        Transform transforms[6] =
        {
            LookRotation(haven->spectatorCamera, float3(1,  0,  0), float3(0, 0, 1)),
            LookRotation(haven->spectatorCamera, float3(-1,  0,  0), float3(0, 0, 1)),
            LookRotation(haven->spectatorCamera, float3(0, -1,  0), float3(0, 0, 1)),
            LookRotation(haven->spectatorCamera, float3(0,  1,  0), float3(0, 0, 1)),
            LookRotation(haven->spectatorCamera, float3(0,  0, -1), float3(0, 1, 0)),
            LookRotation(haven->spectatorCamera, float3(0,  0,  1), float3(0, 1, 0))
        };

        for (int i = 0; i < 6; i++)
        {
            Rendering::SetRenderTarget(cubeTexture[i], "Probe Capture");
            if (i == 0) Rendering::DrawClear(float3(1, 0, 0)); // 1, 0, 0
            if (i == 1) Rendering::DrawClear(float3(0, 1, 1)); // 0, 1, 0
            if (i == 2) Rendering::DrawClear(float3(1, 0, 1)); // 0, 0, 1
            if (i == 3) Rendering::DrawClear(float3(0, 1, 0)); // 0, 1, 1
            if (i == 4) Rendering::DrawClear(float3(1, 1, 0)); // 1, 0, 1
            if (i == 5) Rendering::DrawClear(float3(0, 0, 1)); // 1, 1, 0
            Rendering::DrawClear(haven->clearColor);

            Transform t = transforms[i];
            t.position = startPos;
            Rendering::DrawScene(PerspectiveCamera(t, 90, 1, 100));
        }
        Rendering::SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
    }

    void PackCubemap(Texture* target, Texture* source[6])
    {
        if (!target)
            return;

        for (int i = 0; i < 6; i++)
        {
            if (!source[i])
                return;
        }
        Rendering::SetRenderTarget(target, "Probe Capture");
        Rendering::DrawClear(float3(0, 1, 0));
        CreateMaterialLocal(octUnwrap, assets->reflectionProbeCubemapToOct, reflectionProbeCubemapToOct);
        octUnwrap->cubeTexture0 = source[0];
        octUnwrap->cubeTexture1 = source[1];
        octUnwrap->cubeTexture2 = source[2];
        octUnwrap->cubeTexture3 = source[3];
        octUnwrap->cubeTexture4 = source[4];
        octUnwrap->cubeTexture5 = source[5];
        octUnwrap->mesh = assets->ui_quad;
        Rendering::DrawMesh(octUnwrap);

        Rendering::SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
    }

    // 5x5 tap blur, good for dropping texture resolution by 4x.
    void Downsize4x(Texture* source, Texture* target)
    {
        if (!source)
            return;
        if (!target)
            return;

        Rendering::SetRenderTarget(target, "Probe Capture");
        Rendering::DrawClear(float3(0, 1, 0));
        CreateMaterialLocal(octUnwrap, assets->downsize4x, downsize4x);
        octUnwrap->colorTexture = source;
        octUnwrap->resolution = target->sizeX;
        octUnwrap->mesh = assets->ui_quad;
        Rendering::DrawMesh(octUnwrap);
        Rendering::SetRenderTarget(haven->SwapBuffer, "Rendertarget Reset");
    }
}

#include "entities/entities.cpp"

void printTransform(Transform t2)
{
    haven->printf("pos  : (%.2f, %.2f, %.2f)\n", t2.position.x, t2.position.y, t2.position.z);
    haven->printf("right  : (%.2f, %.2f, %.2f)\nforward: (%.2f, %.2f, %.2f)\nup     : (%.2f, %.2f, %.2f)\n",
        t2.right.x, t2.right.y, t2.right.z,
        t2.forward.x, t2.forward.y, t2.forward.z,
        t2.up.x, t2.up.y, t2.up.z);
}

#include "scene_gryphkiss.cpp"
#include "scene_cornellbox.cpp"
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
    gameMemory->frame++;

    EngineState* engineState = (EngineState*)(&(gameMemory->memory));
    haven = engineState;
    globalGameMemory = gameMemory;

    //Assert(gameMemory->memset, "Critical function missing");
    //Assert(gameMemory->memcpy, "Critical function missing");

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

    haven->Resolution = gameMemory->Resolution;

    ArrayClear(haven->renderCommands);
    ArenaReset(&haven->arenaDrawCommands);
    input = &gameMemory->input;

    bool resetGame = false;
    // hit space = smash game memory to 0 = restart game
    if (input->ctrl && input->spaceDown)
    {
        gameMemory->initialized = false;
        StructClear(haven);
    }
    else if (input->spaceDown)
    {
        resetGame = true;
    }

    if (gameMemory->reloadNow)
    {
        ProfilerBeingSample();
        gameMemory->reloadNow = false;
        ArenaReset(&haven->arenaAssets);
        haven->coutner++;

        ArrayClear(haven->globalTextures);
        ArrayClear(haven->sceneTextures);
        ArrayClear(haven->meshes);
        ArrayClear(haven->shaders);
        ArrayClear(haven->sounds);
        ArrayClear(haven->animations);
        ArrayClear(haven->fonts);

        LoadAssets(&haven->assets);
        ProfilerEndSample("Hotreload");
        haven->hAlign = HAlign_center;
        haven->vAlign = VAlign_center;
    }

    haven->platformPrint = gameMemory->platformPrint;
    haven->printf = gameMemory->printf;
    haven->sprintf = gameMemory->sprintf;
    //haven->memset = gameMemory->memset;
    haven->platformTime = gameMemory->platformTime;
    haven->platformReadFile = gameMemory->platformReadFile;
    haven->platformWriteFile = gameMemory->platformWriteFile;
    haven->platformGraphicsLoadTexture = gameMemory->platformGraphicsLoadTexture;
    haven->platformGraphicsLoadMesh = gameMemory->platformGraphicsLoadMesh;
    haven->platformGraphicsLoadShader = gameMemory->platformGraphicsLoadShader;
    haven->platformGraphicsCreateFramebufferTarget = gameMemory->platformGraphicsCreateFramebufferTarget;
    haven->platformGraphicsCreateTextureTarget = gameMemory->platformGraphicsCreateTextureTarget;

    haven->platformGraphicsReadbackTexture = gameMemory->platformGraphicsReadbackTexture;
    haven->platformGraphicsReadbackTextureHDR = gameMemory->platformGraphicsReadbackTextureHDR;

    //Assert(haven->platformPrint, "Critical function missing");
    //Assert(haven->printf, "Critical function missing");
    //Assert(haven->sprintf, "Critical function missing");
    //Assert(haven->platformTime, "Critical function missing");
    //Assert(haven->platformReadFile, "Critical function missing");
    //Assert(haven->platformWriteFile, "Critical function missing");
    //Assert(haven->platformGraphicsLoadTexture, "Critical function missing");
    //Assert(haven->platformGraphicsLoadMesh, "Critical function missing");
    //Assert(haven->platformGraphicsLoadShader, "Critical function missing");
    //Assert(haven->platformGraphicsCreateFramebufferTarget, "Critical function missing");
    //Assert(haven->platformGraphicsCreateTextureTarget, "Critical function missing");

    bool firstFrame = false;
    if (!gameMemory->initialized)
    {
        firstFrame = true;
        haven->coutner = 0;
        gameMemory->initialized = true;

        ArenaInitialize(&haven->arenasArena, sizeof(haven->arenasArenaData), haven->arenasArenaData);

        ArenaInitialize(&haven->arenaAssets,              Megabytes(512), (uint8*)ArenaPushBytes(&haven->arenasArena,  Megabytes(512),   "Assets",               true), "Assets");
        ArenaInitialize(&haven->arenaDrawCommands,        Megabytes(256), (uint8*)ArenaPushBytes(&haven->arenasArena,  Megabytes(256),   "Draw Commands",        true), "Draw Commands");
        ArenaInitialize(&haven->arenaGlobalDrawCommands,  Megabytes(64),  (uint8*)ArenaPushBytes(&haven->arenasArena,  Megabytes(64),    "Global Draw Commands", true), "Global Draw Commands");
        ArenaInitialize(&haven->arenaEngineState,         Megabytes(64),  (uint8*)ArenaPushBytes(&haven->arenasArena,  Megabytes(64),    "Engine State",         true), "Engine State");
        ArenaInitialize(&haven->arenaScene,               Megabytes(64),  (uint8*)ArenaPushBytes(&haven->arenasArena,  Megabytes(64),    "Scene",                true), "Scene");
        
        haven->profilingData = ArenaPushStruct(&haven->arenaEngineState, ProfilingData, "Profiling Data");
        haven->uiMeshData = ArenaPushStruct(&haven->arenaEngineState, UIMeshData, "UI Mesh Data");

        for (int i = 0; i < ArrayCapacity(haven->soundChannels); i++)
        {
            haven->soundChannels[i] = ArenaPushStruct(&haven->arenaEngineState, SoundChannel, "Sound Channel");
        }

        gameMemory->renderCommands = haven->renderCommands;

        LoadAssets(&haven->assets);
        gameMemory->missingTexture = assets->missingTexture;
        gameMemory->missingMesh = assets->missingMesh;



        CreateMaterialGlobal(haven->axesMaterial, assets->unlit, unlit);
        haven->axesMaterial->ColorTexture = assets->texAxes;
        haven->axesMaterial->Color = float3(1, 1, 1);
        haven->axesMaterial->BackFaceCulling = true;

        CreateMaterialGlobal(haven->boneMaterial, assets->unlit, unlit);
        haven->boneMaterial->shader = assets->unlit;
        haven->boneMaterial->ColorTexture = assets->white;
        haven->boneMaterial->Color = float3(38.0f / 255.0f, 0, 67.0f / 255.0f);

        haven->boneMaterial->Wireframe = true;
        haven->boneMaterial->BackFaceCulling = false;


        haven->SwapBuffer = Rendering::CreateFramebufferTarget(engineState, true);

        haven->stringMesh = ArrayAddNew(haven->meshes);

        haven->spectatorCamera = Transform(float3(-2, -2, 0), float3(0.5, 0.5, -0.5), vectorUp, vectorOne);

        soundStart(gameMemory);

        Editor::Start();

        //Gryphkiss::Start();
        CornellBox::Start();

        profilerStart();


        CreateMaterialGlobal(haven->red, assets->unlit, unlit);
        haven->red->ColorTexture = assets->white;
        haven->red->Color = float3(1, 0, 0);
        haven->red->BackFaceCulling = true;

        CreateMaterialGlobal(haven->green, assets->unlit, unlit);
        haven->green->ColorTexture = assets->white;
        haven->green->Color = float3(0, 1, 0);
        haven->green->BackFaceCulling = true;

        CreateMaterialGlobal(haven->blue, assets->unlit, unlit);
        haven->blue->ColorTexture = assets->white;
        haven->blue->Color = float3(0, 0, 1);
        haven->blue->BackFaceCulling = true;

        return;
    }
    if (resetGame)
    {
        ArrayClear(haven->sceneTextures);
        ArrayClear(haven->sceneMaterials);
        ArrayClear(haven->entities);
        ArenaReset(&haven->arenaScene);
        //Gryphkiss::Start();
        CornellBox::Start();

    }
    Transform headLocal;
    headLocal = input->eyeLeft;
    headLocal.position = float3(0, 0, 0);
    headLocal.position += input->eyeLeft.position;
    headLocal.position += input->eyeRight.position;
    headLocal.position *= 0.5f;
    // transform it to world-space
    input->head = LocalToWorld(headLocal, input->playspace);

    soundUpdate(gameMemory);

    Rendering::SetRenderTarget(haven->SwapBuffer);
    haven->clearColor = float3(0.251, 0.298, 0.373);
    Rendering::DrawClear(haven->clearColor);


    float2 pos = float2(0, 0);
    haven->uiPos = float2(0, 0);
    
    Drawing::DrawText("G: toggle editor");
    Drawing::DrawText("P: toggle profiling");
    Drawing::DrawText("space: reset");
    Drawing::DrawText("ctrl space: reload and reset");
    
    Editor::Update();
    gameMemory->spectatorCamera = haven->spectatorCamera;

    //Gryphkiss::Update();
    CornellBox::Update();
    profilerUpdate();

    if (input->faceButtonLeftDown || input->gDown)
    {
        haven->editor = !haven->editor;
    }
    if (input->faceButtonRightDown || input->pDown)
    {
        haven->profiling = !haven->profiling;
    }

    // Mouse cursor
    CreateMaterialLocal(mouseCursorCommand, assets->UIShader, UIShader);
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


    CreateMaterialLocal(uiCommand, assets->UIShader, UIShader);
    uiCommand->mesh = haven->stringMesh;
    uiCommand->transform = transformIdentity;
    uiCommand->BackFaceCulling = false;
    //command->Wireframe = false;
    //command->DisableDepthTest = true;
    uiCommand->blendMode = BlendMode_Alpha;
    uiCommand->SpriteFont = assets->SpriteFont;
    uiCommand->FontTexture = assets->FontTexture;
    //uiCommand->FontBoldTexture = assets->font;
    
    uiCommand->VRCameraPosition = input->head.position;
    uiCommand->VRCameraForward = input->head.forward;
    uiCommand->VRCameraUp = input->head.up;
    uiCommand->VRCameraRight = input->head.right;

    uiCommand->SpectatorCameraPosition = haven->spectatorCamera.position;
    uiCommand->SpectatorCameraForward = haven->spectatorCamera.forward;
    uiCommand->SpectatorCameraUp = haven->spectatorCamera.up;
    uiCommand->SpectatorCameraRight = haven->spectatorCamera.right;

    // Clear depth so UI is drawn on top.
    Rendering::DrawMesh(uiCommand);
    haven->uiMeshData->quadCount = 0;

    ProfilerEndSample("GryphKissVR");
    
    ProfilerEnd(engineState);
    haven->timeEnd = haven->platformTime();
    haven->internalTime = ((haven->timeEnd - haven->timeStart) * 100) / 1000;

    Rendering::SetRenderTarget(0);
    Rendering::DrawClear();

    CreateMaterialLocal(finalOutputCommand, assets->postProcessTest, postProcessTest);
    finalOutputCommand->mesh = assets->ui_quad;

    finalOutputCommand->ColorTexture = haven->SwapBuffer;
    finalOutputCommand->TexRipples = haven->waterRipplesCurrent;

    Rendering::DrawMesh(finalOutputCommand);

    gameMemory->renderCommands_count = haven->renderCommands_count;

}

#endif

