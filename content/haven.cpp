    
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


struct GameState;
struct ProfilingData;
struct UIMeshData;
struct SoundChannel;
struct Material_unlit;

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

    ArrayCreate(Texture, textures, 100);
    ArrayCreate(Mesh, meshes, 100);
    ArrayCreate(Shader, shaders, 100);
    ArrayCreate(Sound, sounds, 100);
    ArrayCreate(Animation, animations, 100);
    ArrayCreate(Font, fonts, 5);

    ArrayCreate(RenderCommand, renderCommands, 20000);
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

    GameState* gameState;

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

#include "entities/entities.cpp"

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


void LoadAssets(Assets* assets)
{
#define assetRegistryLoad(function, type, name, filename) assets->name = function(filename)
#define assetRegistryLoadShader(function, type, name, filename) assets->name = function(filename); SetupShader_##name(assets->name); assets->name->GLID = haven->platformGraphicsLoadShader(assets->name)
#include "assetRegistry.cpp"
#undef assetRegistryLoadShader
#undef assetRegistryLoad
}

//assets->
//Material_

#define CreateMaterialGlobal(name, type) \
    name = (PASTE(Material_, type)*)ArenaPushStruct(&haven->arenaGlobalDrawCommands, PASTE(Material_, type), ""); \
    name->shader = PASTE(assets->,type); \
    name->BackFaceCulling = true; \
    name->Wireframe = false; \
    SetShaderDefaults(name); \

#define CreateMaterialLocal(name, type) \
    PASTE(Material_, type) name##Data = {}; \
    PASTE(Material_, type)* name = &name##Data; \
    name->shader = PASTE(assets->,type); \
    name->transform = Transform(vectorZero, vectorForward, vectorUp, vectorOne); \
    name->BackFaceCulling = true; \
    name->Wireframe = false; \
    SetShaderDefaults(name); \


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

Texture* CreateFramebufferTarget(EngineState* engineState)
{
    Texture* result = ArrayAddNew(haven->textures);
    result->isFramebufferTarget = true;
    haven->platformGraphicsCreateFramebufferTarget(result);
    return result;
}

Texture* CreateTextureTarget(int sizeX, int sizeY, bool clamp = false)
{
    Texture* result = ArrayAddNew(haven->textures);
    result->isTextureTarget = true;
    result->width = sizeX;
    result->height = sizeY;
    result->aspectRatio = (float)sizeX / (float)sizeY;
    haven->platformGraphicsCreateTextureTarget(result, clamp);
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
        ArenaReset(&haven->arenaAssets);
        haven->coutner++;

        ArrayClear(haven->textures);
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



        CreateMaterialGlobal(haven->axesMaterial, unlit);
        haven->axesMaterial->ColorTexture = assets->texAxes;
        haven->axesMaterial->Color = float3(1, 1, 1);
        haven->axesMaterial->BackFaceCulling = true;

        CreateMaterialGlobal(haven->boneMaterial, unlit);
        haven->boneMaterial->shader = assets->unlit;
        haven->boneMaterial->ColorTexture = assets->white;
        haven->boneMaterial->Color = float3(38.0f / 255.0f, 0, 67.0f / 255.0f);

        haven->boneMaterial->Wireframe = true;
        haven->boneMaterial->BackFaceCulling = false;


        haven->SwapBuffer = CreateFramebufferTarget(engineState);
        haven->waterRipples0 = CreateTextureTarget(256, 256);
        haven->waterRipples1 = CreateTextureTarget(256, 256);

        haven->stringMesh = ArrayAddNew(haven->meshes);

        haven->spectatorCamera = Transform(float3(-2, -2, 0), float3(0.5, 0.5, -0.5), vectorUp, vectorOne);


        soundStart(gameMemory);

        editorStart();
        
        gryphkissStart();

        profilerStart();


        CreateMaterialGlobal(haven->red, unlit);
        haven->red->ColorTexture = assets->white;
        haven->red->Color = float3(1, 0, 0);
        haven->red->BackFaceCulling = true;

        CreateMaterialGlobal(haven->green, unlit);
        haven->green->ColorTexture = assets->white;
        haven->green->Color = float3(0, 1, 0);
        haven->green->BackFaceCulling = true;

        CreateMaterialGlobal(haven->blue, unlit);
        haven->blue->ColorTexture = assets->white;
        haven->blue->Color = float3(0, 0, 1);
        haven->blue->BackFaceCulling = true;

        return;
    }


    soundUpdate(gameMemory);

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

        CreateMaterialLocal(copyTexture, textureCopyShader);
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

    CreateMaterialLocal(rippleSim, rippleSimShader);
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
    haven->clearColor = float3(0.251, 0.298, 0.373);
    DrawClear(haven->clearColor);


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
    CreateMaterialLocal(waterPlane, unlit);
    waterPlane->ColorTexture = haven->waterRipplesCurrent;
    waterPlane->Color = float3(1.0f, 1.0f, 1.0f);
    waterPlane->mesh = assets->ui_quad;
    waterPlane->BackFaceCulling = true;
    waterPlane->transform = identity;
    DrawMesh(waterPlane, assets->ui_quad, transform(center));


    //CreateMaterialLocal(fontTest, unlit);
    //fontTest->ColorTexture = assets->FontRegular;
    //fontTest->Color = float3(1.0f, 1.0f, 1.0f);
    //DrawMesh(fontTest, assets->ui_quad, transform(center + float3(0,0,2)));

    haven->hAlign = HAlign_left;
    haven->vAlign = VAlign_down;

    //if (DrawButton("HAlign_right"))
    //    haven->hAlign = HAlign_right;
    //if (DrawButton("HAlign_center"))
    //    haven->hAlign = HAlign_center;
    //if (DrawButton("HAlign_left"))
    //    haven->hAlign = HAlign_left;
    //
    //if (DrawButton("VAlign_up"))
    //    haven->vAlign = VAlign_up;
    //if (DrawButton("VAlign_center"))
    //    haven->vAlign = VAlign_center;
    //if (DrawButton("VAlign_down"))
    //    haven->vAlign = VAlign_down;

    char* lorem = "What is #88#*Lorem Ipsum*#ff#?\n\n*Lorem Ipsum* is simply #ff000088#dummy#ffffffff# text of the printing and typesetting industry. *Lorem Ipsum* #10#has #20#been #30#the #40#industry's #50#standard #60#dummy #70#text #80#ever #90#since #a0#the#b0# 1500s, #c0#when #d0#an #e0#unknown #f0#printer took a galley of type and scrambled it to make a type specimen book. **It has survived not only five centuries**, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing#ff0000# *Lorem #ffbf00#Ipsum* #80ff00#passages, #00ff40#and #00ffff#more #0040ff#recently #8000ff#with #ff00bf#desktop #ffffff#publishing software like Aldus PageMaker including versions of *Lorem Ipsum*.";
    char* lorem2 = "What is Lorem Ipsum\n\nLorem Ipsum is simply dummy text \nof the printing and typesetting industry. \nLorem Ipsum has been the industry's \nstandard dummy text ever since the \n1500s, when an unknown printer took \na galley of type and scrambled it to \nmake a type specimen book. It has survived \nnot only five centuries, but also the leap \ninto electronic typesetting, remaining \nessentially unchanged. It was popularised \nin the 1960s with the release of Letraset \nsheets containing Lorem Ipsum passages, \nand more recently with desktop publishing \nsoftware like Aldus PageMaker including \nversions of Lorem Ipsum.";
    char* lorem3 = "What is Lorem Ipsum\n\nLorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";
    char* lorem4 = "What is Lorem Ipsum\n\nLorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, **remaining essentially unchanged.** It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";

    //DrawFontMain("Barn bam, The quick brown fox jumps over the lazy dog");
    //DrawFontMain("Hello, this is a test sentence.\nhttps://byork.torvid.net/B-Yorked.htm\ngryph@torvid.net\n$33.99 ((x^2) - 3*4) / 187.0\nvoid main(char* s) { return 0; }\n$!?#&_%\'\",.;:^|`~=<>+-*/\\(){}[]@", float2(400, 420), 200);

    DrawFont2D("@", input->mousePos, 500, 600, haven->hAlign, haven->vAlign);
    //
    //DrawFont2D(lorem4, float2(400, 400), 300, 700, haven->hAlign, haven->vAlign);
    //DrawFontCameraFacing(lorem4, center + float3(0, 0, 3), 0.75, 2.0f * 0.75f, haven->hAlign, haven->vAlign);
    //DrawFont(lorem4, transform(center + float3(-1, 0, 2)), 1.0, 2.0f, haven->hAlign, haven->vAlign);

    //DrawFont2D("LLL**LLL**", float2(700, 200), 800, 400, haven->hAlign, haven->vAlign);
    //
    //DrawText(lorem2, float2(200, 200));
    //DrawText3D(lorem2, center + float3(1, 0, 3));
    //
    //DrawFontMain("aaaaaaaa\naaaaaaaaaaaaaaaa\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", float2(400, 420), 200);
    //DrawFontMain("^v^ uvu ovo >v> <v< ;v; :> >:3 >:D");
    //
    //DrawFontMain("Hello World", float2(100, 100), 50);
    //DrawFontMain("Hello World", float2(100, 120), 100);
    //DrawFontMain("Hello World", float2(100, 180), 500);
    //DrawFontMain("Hello World", float2(100, 300), 1000);
    //DrawFontMain("Hello World", float2(100, 500), 2000);
    //DrawFont3D(lorem, center + float3(0, 8, 0), 2, wrap, haven->hAlign, haven->vAlign);
    //DrawText3D("Hello World", center + float3(0, 0, 2), 2);

    DrawText("G: toggle editor");
    DrawText("P: toggle profiling");
    
    editorUpdate();
    gameMemory->spectatorCamera = haven->spectatorCamera;

    gryphkissUpdate();
    profilerUpdate();

    if (input->faceButtonLeftDown || input->gDown)
    {
        haven->editor = !haven->editor;
    }
    if (input->pDown)
    {
        haven->profiling = !haven->profiling;
    }

    // Mouse cursor
    CreateMaterialLocal(mouseCursorCommand, UIShader);
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

    CreateMaterialLocal(uiCommand, UIShader);
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
    DrawMesh(uiCommand, false);
    haven->uiMeshData->quadCount = 0;

    ProfilerEndSample("GryphKissVR");
    
    ProfilerEnd(engineState);
    haven->timeEnd = haven->platformTime();
    haven->internalTime = ((haven->timeEnd - haven->timeStart) * 100) / 1000;

    SetRenderTarget(0);
    DrawClear();

    CreateMaterialLocal(finalOutputCommand, postProcessTest);
    finalOutputCommand->mesh = assets->ui_quad;

    finalOutputCommand->ColorTexture = haven->SwapBuffer;
    finalOutputCommand->TexRipples = haven->waterRipplesCurrent;

    DrawMesh(finalOutputCommand);

    gameMemory->renderCommands_count = haven->renderCommands_count;
}

#endif

