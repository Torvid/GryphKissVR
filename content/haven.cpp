
#pragma once

#include "haven_platform.h"

#include "math.cpp"
#include "memory.cpp"
#include "string.cpp"


// X Macro for generating enums and strings
#define ENUM(name, a) name##_##a,
#define STRINGS(name, a) #a,
#define MakeEnum(name, tableMaker) \
enum name \
{ \
    tableMaker(name, ENUM) \ 
    name##_COUNT\
}; \
char* name##Strings[name##_COUNT + 1] = \
{ \
    tableMaker(name, STRINGS) \
    "" \
}; \
char* name##ToString(name x) { return name##Strings[x]; };


// Example 1
#define MyEnumTable(n, X) \
    X(n, None) \
    X(n, PlaceComponent) \
    X(n, PlaceWire) \
    X(n, DragWire)
MakeEnum(MyEnum, MyEnumTable);

// Example 2
#define AnimalEnumTable(n, X) \
    X(n, Dragon) \
    X(n, Gryphon) \
    X(n, Dog) \
    X(n, Cat)
MakeEnum(AnimalEnum, AnimalEnumTable);



enum SoundFalloffType
{
    SoundFalloffType_Linear,
    SoundFalloffType_Exponential,
    SoundFalloffType_None,
};

struct SoundChannel
{
    bool playing;
    int currentSample;
    Sound* sound;
    bool looping;
    float volume;
    bool is3D;
    float3 position;
};



struct RatData
{
    Mesh* model;
    Texture* texture;

    // Draw to this
    Texture* ratBuffer;

    // Add to this
    Texture* ratBufferCopyTo;
};

struct GameState;

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
    ArrayCreate(Mesh, stringMeshes, 100);
    ArrayCreate(Animation, animations, 100);
    ArrayCreate(RenderCommand, renderCommands, 5000);

    Texture* missingTexture;
    Mesh* missingMesh;
    Shader* missingShader;

    Sound* TestSound_44k_mono;
    Sound* TestSound_48k_mono;
    Sound* TestSound_22k_mono;
    Sound* TestSound_44k_stereo;

    Texture* texture0;
    Texture* texture1;
    Texture* texture2;
    Texture* texWhite;
    Texture* fontTexture;
    Texture* testImage;

    Mesh* StrawPileMesh;

    Texture* SwapBuffer0;
    Texture* SwapBuffer1;

    Texture* waterRipples0;
    Texture* waterRipples1;


    Texture* waterRipplesPrevious;
    Texture* waterRipplesCurrent;


    Mesh* monkey;
    Mesh* bendyMonkey;
    Animation* bendyMonkeyAnimation;

    Mesh* torvidTest;
    Animation* torvidTestAnimation;

    Sound* torvidSound;

    Mesh* box;
    Mesh* axes;
    Shader* unlit;
    Shader* defaultlit;

    Shader* UIShader;
    Mesh* ui_circle;
    Mesh* ui_quad;
    Mesh* ui_textTest;
    Shader* postProcessShader;
    Shader* textureCopyShader;
    Shader* rippleSimShader;

    Mesh* boneSphere;
    Mesh* bonePyramid;

    Transform spectatorCamera;

    SoundChannel soundChannels[25];

    bool headsetView;

    MemoryArena arenasArena;
    uint8 arenasArenaData[Megabytes(1024)];

    MemoryArena arenaHotreload;
    MemoryArena arenaDrawCommands;
    MemoryArena arenaDrawTextCommands;
    MemoryArena arenaGlobalDrawCommands;
    MemoryArena arenaGameState;

    uint8 scratchBuffer[Megabytes(256)];

    RatData ratData;
    int monkeyFrame;
    float torvidFrame;
    bool editor;
    bool profiling;

    int internalTime;
    int externalTime;
    int timeStart;
    int timeEnd;

    GameState* gameState;

};

#include "loadMesh.cpp"
#include "loadAnimation.cpp"
#include "loadShader.cpp"
#include "loadSound.cpp"
#include "loadTexture.cpp"


#define Cpp 1


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





#define float2(x, y) ctor_float2((x), (y))
#define float3(x, y, z) ctor_float3((x), (y), (z))

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


void PlaySound(EngineState* engineState, Sound* sound, float volume = 1.0f, bool looping = false)
{
    SoundChannel* foundChannel = 0;
    for (int i = 0; i < ArrayCapacity(engineState->soundChannels); i++)
    {
        if (!engineState->soundChannels[i].playing)
        {
            foundChannel = &engineState->soundChannels[i];
            break;
        }
    }

    if (!foundChannel)
        return;

    StructClear(foundChannel);

    foundChannel->playing = true;
    foundChannel->sound = sound;
    foundChannel->volume = volume;
    foundChannel->looping = looping;

}

#include "ui.cpp"
#include "profiling.cpp"

void UpdateEditorCamera(EngineState* engineState, Input* input)
{
    if (!input->mouseRight)
        return;

    float mouseDeltaX = input->mousePosDelta.x;
    float mouseDeltaY = input->mousePosDelta.y;
    if (engineState->spectatorCamera.up.z < 0)
    {
        mouseDeltaX = -mouseDeltaX;
    }
    engineState->spectatorCamera = rotate(engineState->spectatorCamera, float3(0, 0, 1), -mouseDeltaX * 0.005f);
    engineState->spectatorCamera = rotate(engineState->spectatorCamera, engineState->spectatorCamera.right, -mouseDeltaY * 0.005f);

    int x = input->w ? 1 : 0 + input->s ? -1 : 0;
    int y = input->d ? 1 : 0 + input->a ? -1 : 0;
    int z = input->e ? 1 : 0 + input->q ? -1 : 0;

    float3 Movement = float3(0, 0, 0);
    Movement += engineState->spectatorCamera.forward * x;
    Movement += engineState->spectatorCamera.right * y;
    Movement += engineState->spectatorCamera.up * z;

    if (length(Movement) > 0)
        Movement = normalize(Movement);

    float speed = 4;
    if (input->shift)
        speed = 16;
    
    engineState->spectatorCamera.position += Movement * input->deltaTime * speed;
}


Transform ApplyTransform(Transform t, Transform monkeyRotation)
{
    Transform t2 = {};
    t2.scale = t.scale;
    float3 d0 = float3(monkeyRotation.right.x, monkeyRotation.forward.x, monkeyRotation.up.x);
    float3 d1 = float3(monkeyRotation.right.y, monkeyRotation.forward.y, monkeyRotation.up.y);
    float3 d2 = float3(monkeyRotation.right.z, monkeyRotation.forward.z, monkeyRotation.up.z);
    t2.position.x = dot(t.position, d0);
    t2.position.y = dot(t.position, d1);
    t2.position.z = dot(t.position, d2);
    t2.position += monkeyRotation.position;

    t2.right.x = -dot(d0, t.right);
    t2.right.y = -dot(d1, t.right);
    t2.right.z = -dot(d2, t.right);

    t2.forward.x = dot(d0, t.forward);
    t2.forward.y = dot(d1, t.forward);
    t2.forward.z = dot(d2, t.forward);

    t2.up.x = dot(d0, t.up);
    t2.up.y = dot(d1, t.up);
    t2.up.z = dot(d2, t.up);
    return t2;
}

#include "gryphkiss.cpp"


extern "C" __declspec(dllexport) void gameUpdateAndRender(GameMemory* memory)
{
    EngineState* engineState = (EngineState*)(&(memory->memory));
    engineState->timeStart = memory->platformTime();
    engineState->externalTime = ((engineState->timeStart - engineState->timeEnd) * 100) / 1000;
    ArrayClear(engineState->stringMeshes);
    //ArrayClear(memory->renderCommands);
    ArrayClear(engineState->renderCommands);
    ArenaReset(&engineState->arenaDrawCommands);
    ArenaReset(&engineState->arenaDrawTextCommands);
    Input* input = &memory->input;

    // hit space = smash game memory to 0 = restart game
    if (input->spaceDown)
    {
        memory->initialized = false;
        StructClear(engineState);
    }

    if (memory->reloadNow)
    {
        memory->reloadNow = false;
        ArenaReset(&engineState->arenaHotreload);

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
    GameState* gameState = engineState->gameState;
    if (!memory->initialized)
    {
        memory->initialized = true;

        
        ArenaInitialize(&engineState->arenasArena, sizeof(engineState->arenasArenaData), engineState->arenasArenaData);

        ArenaInitialize(&engineState->arenaHotreload,           Megabytes(512), (uint8*)ArenaPushBytes(&engineState->arenasArena, Megabytes(512),   "Hotreload",             true), "Hotreload");
        ArenaInitialize(&engineState->arenaDrawCommands,        Megabytes(64),  (uint8*)ArenaPushBytes(&engineState->arenasArena, Megabytes(64),    "Draw Commands",         true), "Draw Commands");
        ArenaInitialize(&engineState->arenaDrawTextCommands,    Megabytes(64),  (uint8*)ArenaPushBytes(&engineState->arenasArena, Megabytes(64),    "Text Draw Commands",    true), "Text Draw Commands");
        ArenaInitialize(&engineState->arenaGlobalDrawCommands,  Megabytes(64),  (uint8*)ArenaPushBytes(&engineState->arenasArena, Megabytes(64),    "Global Draw Commands",  true), "Global Draw Commands");
        ArenaInitialize(&engineState->arenaGameState,           Megabytes(64),  (uint8*)ArenaPushBytes(&engineState->arenasArena, Megabytes(64),    "Game State",            true), "Game State");

        engineState->gameState = ArenaPushStruct(&engineState->arenaGameState, GameState, "");
        gameState = engineState->gameState;


        engineState->platformPrint = memory->platformPrint;
        engineState->printf = memory->printf;
        engineState->sprintf = memory->sprintf;
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




        memory->missingTexture = FileReadTexture(engineState, "missing.tga");
        memory->missingMesh = FileReadMesh(engineState, "missing.obj");
        engineState->missingTexture = memory->missingTexture;
        engineState->missingMesh = memory->missingMesh;
        engineState->missingShader = memory->missingShader;

        //engineState->TestSound_44k_mono = FileReadSound(engineState, "GlassSmash_44k_mono.wav");
        //engineState->TestSound_48k_mono = FileReadSound(engineState, "Groan_48k_mono.wav");
        engineState->TestSound_22k_mono = FileReadSound(engineState, "HitWoodDome_22k_mono.wav");
        //engineState->TestSound_44k_stereo = FileReadSound(engineState, "MusicTheme_44k_stereo.wav");


        

        engineState->texture0 = FileReadTexture(engineState, "axes.tga");
        engineState->texture1 = FileReadTexture(engineState, "wuff.tga");
        engineState->texture2 = FileReadTexture(engineState, "coal.tga");
        engineState->texWhite = FileReadTexture(engineState, "white.tga");
        engineState->fontTexture = FileReadTexture(engineState, "font.tga");
        engineState->testImage = FileReadTexture(engineState, "testImage.tga");

        engineState->axes = FileReadMesh(engineState, "axes.obj");
        engineState->monkey = FileReadMesh(engineState, "monkey.obj");
        

        engineState->torvidTest = FileReadMesh(engineState, "torvidTest.mesh");
        engineState->torvidTestAnimation = FileReadAnimation(engineState, "torvidTest.anim");
        engineState->torvidSound = FileReadSound(engineState, "Torvid_Gryphon.wav");


        engineState->box = FileReadMesh(engineState, "box.obj");
        engineState->StrawPileMesh = FileReadMesh(engineState, "barn/StrawPile.obj");

        engineState->ui_circle = FileReadMesh(engineState, "ui_circle.obj");
        engineState->ui_quad = FileReadMesh(engineState, "ui_quad.obj");


        CreateMaterialGlobal(engineState, gameState->axesMaterial, engineState->unlit,  Material_unlit);

        gameState->axesMaterial->ColorTexture = engineState->texture0;
        gameState->axesMaterial->Color = float3(1, 1, 1);


        gameState->axesMaterial->BackFaceCulling = true;

        CreateMaterialGlobal(engineState, gameState->boneMaterial, engineState->unlit, Material_unlit);
        gameState->boneMaterial->shader = engineState->unlit;
        gameState->boneMaterial->ColorTexture = engineState->texWhite;
        gameState->boneMaterial->Color = float3(38.0f / 255.0f, 0, 67.0f / 255.0f);

        gameState->boneMaterial->Wireframe = true;
        gameState->boneMaterial->BackFaceCulling = false;


        CreateMaterialGlobal(engineState, gameState->red, engineState->unlit, Material_unlit);

        gameState->red->ColorTexture = engineState->texWhite;
        gameState->red->Color = float3(1, 0, 0);
        gameState->red->BackFaceCulling = true;

        CreateMaterialGlobal(engineState, gameState->green, engineState->unlit, Material_unlit);
        gameState->green->ColorTexture = engineState->texWhite;
        gameState->green->Color = float3(0, 1, 0);
        gameState->green->BackFaceCulling = true;

        CreateMaterialGlobal(engineState, gameState->blue, engineState->unlit, Material_unlit);
        gameState->blue->ColorTexture = engineState->texWhite;
        gameState->blue->Color = float3(0, 0, 1);
        gameState->blue->BackFaceCulling = true;

        // Load StrawPile
        CreateMaterialGlobal(engineState, gameState->StrawPileMat, engineState->defaultlit, Material_defaultlit);
        gameState->StrawPileMat->texM1 = FileReadTexture(engineState, "TorvidM1.tga");
        gameState->StrawPileMat->texM2 = FileReadTexture(engineState, "TorvidM2.tga");
        gameState->StrawPileMat->texM3 = FileReadTexture(engineState, "TorvidM3.tga");
        gameState->StrawPileMat->texM1Extra = FileReadTexture(engineState, "TorvidFeathersM1.tga");
        gameState->StrawPileMat->texM2Extra = FileReadTexture(engineState, "TorvidFeathersM2.tga");
        gameState->StrawPileMat->texM3Extra = FileReadTexture(engineState, "TorvidFeathersM3.tga");
        gameState->StrawPileMat->BackFaceCulling = false;


        engineState->SwapBuffer0 = CreateFramebufferTarget(engineState);
        engineState->waterRipples0 = CreateTextureTarget(engineState, 512, 512);
        engineState->waterRipples1 = CreateTextureTarget(engineState, 512, 512);

        engineState->boneSphere = FileReadMesh(engineState, "bone_sphere.obj");
        engineState->bonePyramid = FileReadMesh(engineState, "bone_pyramid.obj");



        engineState->spectatorCamera = Transform(float3(-2, -2, 0), float3(0.5, 0.5, -0.5), vectorUp, vectorOne);

        return;
    }
    UpdateEditorCamera(engineState, &memory->input);
    memory->spectatorCamera = engineState->spectatorCamera;

    if (engineState->headsetView)
        memory->spectatorCamera = input->head;
    
    for (int i = 0; i < memory->SampleCount; i++)
    {
        memory->Samples[i].left = 0;
        memory->Samples[i].right = 0;
    }

    // Sound
    for (int j = 0; j < ArrayCapacity(engineState->soundChannels); j++)
    {
        for (int i = 0; i < memory->SampleCount; i++)
        {
            SoundChannel* channel = &engineState->soundChannels[j];

            if (!channel->playing)
                break;

            Sound* sound = channel->sound;

            if (!sound)
                break;
            float x = ((input->mousePos.x / 2500.0f) - 0.5) * 2 * 16;
            int s1 = clamp(channel->currentSample - x, 0, (sound->sampleCount - 1));
            int s2 = clamp(channel->currentSample + x, 0, (sound->sampleCount - 1));
            Sample sample1 = sound->data[s1];
            Sample sample2 = sound->data[s2];

            int left = memory->Samples[i].left + (sample1.left * channel->volume * 0.5f);
            int right = memory->Samples[i].right + (sample2.right * channel->volume * 0.5f);

            if (!memory->gameFocused)
            {
                left = 0;
                right = 0;
            }

            memory->Samples[i].left = clamp(left, -32768, 32767);
            memory->Samples[i].right = clamp(right, -32768, 32767);

            channel->currentSample++;
            if (channel->currentSample > (sound->sampleCount - 1)) // went over, loop or stop.
            {
                if (channel->looping)
                    channel->currentSample = 0;
                else
                    channel->playing = false;
            }
        }
    }

    input->head = input->eyeLeft;
    input->head.position = float3(0, 0, 0);
    input->head.position += input->eyeLeft.position;
    input->head.position += input->eyeRight.position;
    input->head.position *= 0.5f;


    if (input->mouseLeftDown || input->faceButtonRight)
    {
        engineState->waterRipplesPrevious = engineState->waterRipples0;
        engineState->waterRipplesCurrent = engineState->waterRipples1;

        // setup
        SetRenderTarget(engineState, engineState->waterRipplesPrevious);
        DrawClear(engineState);

        CreateMaterialLocal(copyTexture, engineState->textureCopyShader, Material_textureCopy);
        copyTexture->mesh = engineState->ui_quad;
        copyTexture->ColorTexture = engineState->texture2;
        DrawMesh(engineState, copyTexture);


        SetRenderTarget(engineState, engineState->waterRipplesCurrent);
        DrawClear(engineState);

        copyTexture->ColorTexture = engineState->texture2;
        DrawMesh(engineState, copyTexture);
    }

    // draw
    SetRenderTarget(engineState, engineState->waterRipplesCurrent);
    DrawClear(engineState);

    
    CreateMaterialLocal(rippleSim, engineState->rippleSimShader, Material_rippleSim);
    rippleSim->mesh = engineState->ui_quad;
    rippleSim->TexPrevious = engineState->waterRipplesPrevious;
    rippleSim->mousePos = input->mousePos / memory->Resolution;
    DrawMesh(engineState, rippleSim);


    // swap
    Texture* temp;
    temp = engineState->waterRipplesPrevious;
    engineState->waterRipplesPrevious = engineState->waterRipplesCurrent;
    engineState->waterRipplesCurrent = temp;

    SetRenderTarget(engineState, engineState->SwapBuffer0);
    DrawClear(engineState, float3(0.5,0,0));

    
    
    

    input->vibrationAmplitudeRight = input->grabRight;


    input->handRight.scale = { 0.1f, 0.1f, 0.1f };
    input->handLeft.scale = { 0.1f, 0.1f, 0.1f };
    input->aimRight.scale = { 0.1f, 0.1f, 0.1f };
    input->aimLeft.scale = { 0.1f, 0.1f, 0.1f };
    if (engineState->editor)
    {
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->handRight);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->handLeft);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->aimRight);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->aimLeft);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->playspaceStage);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->playspaceStageLeft);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->playspaceStageRight);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->playspaceStageLeftRotated);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->playspaceStageRightRotated);
    }

    float3 center = (input->playspaceStageLeft.position + input->playspaceStageRight.position) / 2.0f;
    Transform monkeyRotation = { center +
        float3(-1, 0, 0),
        //float3(-0.1, -1.5f, 0.7f),
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
    waterPlane->mesh = engineState->ui_quad;
    waterPlane->BackFaceCulling = true;
    waterPlane->transform = identity;
    DrawMesh(engineState, waterPlane);


    float animationFPS = 25.0f;
    if (engineState->torvidFrame == 0.0f)
    {
        PlaySound(engineState, engineState->torvidSound, 0.25f);
    }
    engineState->torvidFrame += animationFPS * input->deltaTime;
    if (engineState->torvidFrame > 180.0f)
    {
        engineState->torvidFrame = 0.0f;
    }


    if (engineState->editor)
    {
        DrawMesh(engineState, gameState->red, engineState->box, { float3(0,0,0) + float3(0.25,   0,    0),    { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 0.5,  0.025, 0.025 } });
        DrawMesh(engineState, gameState->green, engineState->box, { float3(0,0,0) + float3(0,    0.25, 0),    { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 0.025, 0.5,  0.025 } });
        DrawMesh(engineState, gameState->blue, engineState->box, { float3(0,0,0) + float3(0,     0,    0.25), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 0.025, 0.025, 0.5  } });


        Transform monkeyRotation2 = { center + float3(0, -6, 0), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 } };
        DrawMesh(engineState, gameState->axesMaterial, engineState->monkey, monkeyRotation2);
        monkeyRotation2 = { center + float3(3, -3, 0), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 } };
        DrawMesh(engineState, gameState->axesMaterial, engineState->monkey, monkeyRotation2);
        monkeyRotation2 = { center + float3(-3, -3, 0), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 } };
        DrawMesh(engineState, gameState->StrawPileMat, engineState->monkey, monkeyRotation2);
    }

    // Set up bones
    for (int i = 0; i < engineState->torvidTestAnimation->boneCount; i++)
    {
        int frameOffset = ((int)engineState->torvidFrame) * engineState->torvidTestAnimation->boneCount;
        Transform t = engineState->torvidTestAnimation->transforms[i + frameOffset];
        // TODO: fix this
        gameState->StrawPileMat->shaderBoneTransforms[i] = t;
    }

    // Draw torvid
    DrawMesh(engineState, gameState->StrawPileMat, engineState->torvidTest, monkeyRotation);


    if (engineState->editor)
    {
        DrawClearDepth(engineState);
        // Draw transforms
        for (int i = 0; i < engineState->torvidTestAnimation->boneCount; i++)
        {
            int frameOffset = ((int)engineState->torvidFrame) * engineState->torvidTestAnimation->boneCount;
            Transform t = engineState->torvidTestAnimation->transforms[i + frameOffset];
            float distanceToParent = 0.5f;
            float boneRadius = 0.03f;
            t.scale = vectorOne * boneRadius;
            DrawMesh(engineState, gameState->boneMaterial, engineState->boneSphere, ApplyTransform(t, monkeyRotation));
            if (!engineState->torvidTest->boneHierarchy[i].parent)
                continue;
    
            Transform t2 = t;
            int parentIndex = engineState->torvidTest->boneHierarchy[i].parent->index;
            float3 parentPos = engineState->torvidTestAnimation->transforms[parentIndex + frameOffset].position;
            distanceToParent = distance(parentPos, t2.position);
            t2.forward = normalize(t2.position - parentPos);
            t2.right = normalize(cross(t2.forward, t2.up));
            t2.up = normalize(cross(t2.forward, t2.right));
            t2.position = parentPos + t2.forward * boneRadius;
            t2.scale = float3(boneRadius, distanceToParent - boneRadius * 2, boneRadius);
    
            DrawMesh(engineState, gameState->boneMaterial, engineState->bonePyramid, ApplyTransform(t2, monkeyRotation));
        }
    
    }

    if (input->faceButtonLeftDown || input->faceButtonRightDown || input->gDown)
    {
        engineState->editor = !engineState->editor;
    }
    if (input->faceButtonLeftDown || input->faceButtonRightDown || input->pDown)
    {
        engineState->profiling = !engineState->profiling;
    }



    DrawClearDepth(engineState);

    // Mouse cursor
    CreateMaterialLocal(mouseCursorCommand, engineState->UIShader, Material_UI);

    mouseCursorCommand->transform = {};
    mouseCursorCommand->mesh = engineState->ui_circle;
    mouseCursorCommand->BackFaceCulling = false;
    mouseCursorCommand->Wireframe = false;
    mouseCursorCommand->DisableDepthTest = true;

    mouseCursorCommand->Position = input->mousePos;
    mouseCursorCommand->ColorTexture = engineState->texWhite;


    mouseCursorCommand->Color = float3(0, 0, 0);
    mouseCursorCommand->Size = float2(9, 9);
    DrawMesh(engineState, mouseCursorCommand);

    mouseCursorCommand->Color = float3(1, 1, 1);
    mouseCursorCommand->Size = float2(6, 6);
    DrawMesh(engineState, mouseCursorCommand);

    // Draw UI
    float2 pos = float2(0, 0);
    Transform textTransform = input->head;
    textTransform.position += textTransform.forward * 3;
    textTransform.position += textTransform.up * 0.25;
    textTransform.position += textTransform.right * -0.25;

    DrawBox(engineState, float2(200, 500), float2(300, 300), float3(0.5, 0.2, 0.2), textTransform);
    DrawBox(engineState, float2(300, 600), float2(300, 300), float3(0.2, 0.5, 0.2), textTransform);
    DrawBox(engineState, float2(400, 700), float2(300, 300), float3(0.2, 0.2, 0.5), textTransform);

    // Update text
    const int tempStringSize = 2048;
    char text[tempStringSize] = {};
    Clear((uint8*)text, tempStringSize);
    StringAppend(text, "G: toggle info");
    StringAppend(text, "\nP: toggle profiling");
    DrawText(engineState, text, &pos, textTransform);
    Clear((uint8*)text, tempStringSize);

    if (engineState->profiling)
    {
        StringAppend(text, "\nPERFORMANCE:");
        StringAppend(text, "\n    Time: ", input->time);
        StringAppend(text, "\n    FPS: ", 1.0f / input->deltaTime);
        StringAppend(text, "\n    Internal Time: ", (int)engineState->internalTime, " microseconds");
        StringAppend(text, "\n    External Time: ", (int)engineState->externalTime, " microseconds");
        StringAppend(text, "\n    Total frame time: ", (int)(input->deltaTime * 1000), " milliseconds");
        DrawText(engineState, text, &pos, textTransform);


        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "\n\nMEMORY:");
        DrawText(engineState, text, &pos, textTransform);

        //DrawArena(memory, engineState, &pos, &engineState->arenaHotreload, textTransform);
        DrawArena(engineState, &pos, &engineState->arenaDrawCommands, textTransform);
        DrawArena(engineState, &pos, &engineState->arenaGlobalDrawCommands, textTransform);
        DrawArena(engineState, &pos, &engineState->arenaGameState, textTransform);
    }

    if (engineState->editor)
    {
        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "\n\nINPUT: ");
        StringAppend(text, "\n    Mouse pos: ", input->mousePos);
        StringAppend(text, "\n    Eye pos: ", input->eyeLeft.position);
        DrawText(engineState, text, &pos, textTransform);

        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "    Left Controller:\n");
        StringAppend(text, "        Pos: ", input->handLeft.position);
        StringAppend(text, "\n        Trigger: ", input->triggerLeft);
        StringAppend(text, "\n        Thumbstick: ", input->thumbstickLeft);
        StringAppend(text, "\n        Face Button: ", input->faceButtonLeft);
        DrawText(engineState, text, &pos, textTransform);

        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "    Right Controller:\n");
        StringAppend(text, "        Pos: ", input->handRight.position);
        StringAppend(text, "\n        Trigger: ", input->triggerRight);
        StringAppend(text, "\n        Thumbstick: ", input->thumbstickRight);
        StringAppend(text, "\n        Face Button: ", input->faceButtonRight);
        DrawText(engineState, text, &pos, textTransform);

        engineState->headsetView = DrawToggle(engineState, input, "Headset View: ", &pos, textTransform, engineState->headsetView);

        if (DrawButton(engineState, input, "Play a sound", &pos, textTransform) || input->faceButtonLeftDown)
        {
            PlaySound(engineState, engineState->TestSound_22k_mono, 1.0f, false);
        }


        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "\n\nSOUND: \n");
        for (int i = 0; i < ArrayCapacity(engineState->soundChannels); i++)
        {
            SoundChannel* channel = &engineState->soundChannels[i];

            if (channel->playing)
            {
                StringAppend(text, "    ", (int)i, ", ");
                StringAppend(text, channel->sound->filename, ", ");
                StringAppend(text, ((float)channel->currentSample / (float)channel->sound->sampleCount), "\n");
            }
        }
        DrawText(engineState, text, &pos, textTransform);


    }





    SetRenderTarget(engineState, 0);
    DrawClear(engineState);

    CreateMaterialLocal(finalOutputCommand, engineState->postProcessShader, Material_PostProcessTest);
    finalOutputCommand->mesh = engineState->ui_quad;

    finalOutputCommand->ColorTexture = engineState->SwapBuffer0;
    finalOutputCommand->TexRipples = engineState->waterRipplesCurrent;

    DrawMesh(engineState, finalOutputCommand);

    engineState->timeEnd = engineState->platformTime();

    engineState->internalTime = ((engineState->timeEnd - engineState->timeStart) * 100) / 1000;

    memory->renderCommands_count = engineState->renderCommands_count;
}
