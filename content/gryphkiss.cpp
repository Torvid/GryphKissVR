#pragma once

#include "haven.cpp"

enum EntityType
{
    EntityType_Hand,
    EntityType_Player,
    EntityType_StaticMesh,
    EntityType_LightBaker,
    EntityType_ReflectionProbe,
    EntityType_Max,
};

// forward declare stuff that shoudl be available in components
void SetLightmap(GameState* gameState, Texture* texture, float3 lightmapMin, float3 lightmapMax, float3 lightmapResolution, float radiosityProbeScale);

#include "entities/staticMesh.cpp"
#include "entities/reflectionProbe.cpp"
#include "entities/lightBaker.cpp"

#include "entities/player.cpp"
#include "entities/hand.cpp"

#define CreateSceneMaterial(name) \
    CreateMaterialGlobal(name, assets->defaultlit, defaultlit); \
    ArrayAdd(haven->sceneMaterials, name);

struct GameState
{
    MemoryArena arenaScene; // memory arena for all the entities currently alive in the scene.

    // Torvid
    float torvidFrame;

    Material_defaultlit* torvidMat;

    // Barn stuff
    Material_defaultlit* barnWallMat;
    Material_defaultlit* barnWallCleanMat;
    Material_defaultlit* barnCeilingMat;
    Material_defaultlit* barnTilesMat;
    Material_defaultlit* StrawPileMat;

    Material_skydomeShader* skydomeMat;

    Hand* leftHand;
    Hand* rightHand;

    Player* player;
    LightBaker* lightBaker;

    Texture* cubemap;
    Texture* lightmap;
};

void SetCubemap(GameState* gameState, Texture* texture)
{
    for (int i = 0; i < ArrayCount(haven->sceneMaterials); i++)
    {
        haven->sceneMaterials[i]->texCubemap = texture;
    }
}

void SetLightmap(GameState* gameState, Texture* texture, float3 lightmapMin, float3 lightmapMax, float3 lightmapResolution, float radiosityProbeScale)
{
    for (int i = 0; i < ArrayCount(haven->sceneMaterials); i++)
    {
        haven->sceneMaterials[i]->texLightmap = texture;
        haven->sceneMaterials[i]->lightmapMin = lightmapMin;
        haven->sceneMaterials[i]->lightmapMax = lightmapMax;
        haven->sceneMaterials[i]->lightmapResolution = lightmapResolution;
        haven->sceneMaterials[i]->radiosityProbeScale = radiosityProbeScale;
    }
}

void gryphkissStart()
{
    haven->gameState = ArenaPushStruct(&haven->arenaEngineState, GameState, "GameState");
    GameState* gameState = haven->gameState;

    ArenaInitialize(&gameState->arenaScene, Megabytes(64), (uint8*)ArenaPushBytes(&haven->arenasArena, Megabytes(64), "Scene", true), "Scene");

    gameState->lightBaker = LightBakerInstantiate();
    ReflectionProbe* reflectionProbe = ReflectionProbeInstantiate(transform(float3(0, 0, 0)));


    // Load Torvid
    CreateSceneMaterial(gameState->torvidMat);
    gameState->torvidMat->texM1 = assets->TorvidM1;
    gameState->torvidMat->texM2 = assets->TorvidM2;
    gameState->torvidMat->BackFaceCulling = true;

    // Create materials and load textures
    CreateSceneMaterial(gameState->barnWallMat);
    gameState->barnWallMat->texM1 = assets->BarnWallM1;
    gameState->barnWallMat->texM2 = assets->BarnWallM2;

    CreateSceneMaterial(gameState->barnWallCleanMat);
    gameState->barnWallCleanMat->texM1 = assets->BarnWallCleanM1;
    gameState->barnWallCleanMat->texM2 = assets->BarnWallCleanM2;

    CreateSceneMaterial(gameState->barnCeilingMat);
    gameState->barnCeilingMat->texM1 = assets->BarnCeilingM1;
    gameState->barnCeilingMat->texM2 = assets->BarnCeilingM2;

    CreateSceneMaterial(gameState->barnTilesMat);
    gameState->barnTilesMat->texM1 = assets->TilesM1;
    gameState->barnTilesMat->texM2 = assets->TilesM2;

    CreateSceneMaterial(gameState->StrawPileMat);
    gameState->StrawPileMat->texM1 = assets->StrawPileM1;
    gameState->StrawPileMat->texM2 = assets->StrawPileM2;

    CreateMaterialGlobal(gameState->skydomeMat, assets->skydomeShader, skydomeShader);
    gameState->skydomeMat->Color = float3(1, 1, 1);
    gameState->skydomeMat->ColorTexture = assets->StrawPileM1;
    //gameState->skydomeMat.text
    //skydomeMat

    SetCubemap(gameState, assets->black);
    SetLightmap(gameState, assets->black, float3(0, 0, 0), float3(1, 1, 1), float3(2, 2, 2), 1.0f);

    gameState->leftHand  = Instantiate(Hand);
    gameState->rightHand = Instantiate(Hand);
    gameState->leftHand->handSide = HandSide_Left;
    gameState->rightHand->handSide = HandSide_Right;
    HandStart(gameState->leftHand);
    HandStart(gameState->rightHand);

    gameState->player = Instantiate(Player);
    PlayerStart(gameState->player);
    
    StaticMeshInstantiate(assets->tonk, gameState->barnWallMat, transform(float3(3, 5, 0), float3(0.1, 0.2, 0.1)));

    // left wall
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            if (y == 1)
            {
                StaticMeshInstantiate(assets->BarnWall02, gameState->barnWallCleanMat, transform(float3(0, x * 2, y), 0, 0, -0.25));
            }
            else
            {
                StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(0, x * 2, y), 0, 0, -0.25));
            }
        }
    }
    // right wall
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(5, x * 2 + 2, y), 0, 0, 0.25));
        }
    }

    // floor
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            StaticMeshInstantiate(assets->tileFloor, gameState->barnTilesMat, transform(float3(x+ 0.5, y+0.5, 0) * 2, float3(1, 1, 1)));
        }
    }

    // front wall
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(x*2+2, 0, y+2), 0, 0, 0.0));
        }
    }
    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(3, 0, 4), 0, 0, 0.0));
    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(5, 0, 4), 0, 0, 0.0));
    StaticMeshInstantiate(assets->BarnWall02, gameState->barnWallCleanMat, transform(float3(1, 0, 0), 0, 0, 0.0));
    StaticMeshInstantiate(assets->BarnWall02, gameState->barnWallCleanMat, transform(float3(1, 0, 1), 0, 0, 0.0));

    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(5, 0, 0), 0, 0, 0.0));
    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(5, 0, 1), 0, 0, 0.0));

    // doors
    //StaticMeshInstantiate(assets->BarnDoor, gameState->barnWallCleanMat, transform(float3(1, 0, 2), 0, 0, 0.0, float3(-1, 1, -1)));
    //StaticMeshInstantiate(assets->BarnDoor, gameState->barnWallCleanMat, transform(float3(3, 0, 0), 0, 0, 0.0));

    // back wall
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(x * 2 , 10, y), 0, 0, 0.5));
        }
    }
    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(1, 10, 4), 0, 0, 0.5));
    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(float3(3, 10, 4), 0, 0, 0.5));

    // ceiling left
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            StaticMeshInstantiate(assets->BarnCeiling01, gameState->barnWallCleanMat, transform(float3(x*0.704f, y*2, x * 0.704f) + float3(0, 2, 3), 0, 0.125, 0.25));
        }
    }

    // ceiling right
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            StaticMeshInstantiate(assets->BarnCeiling01, gameState->barnWallCleanMat, transform(float3(-x * 0.704f, y * 2, x * 0.704f) + float3(5, 0, 3), 0, -0.125, -0.25));
        }
    }

    //StaticMeshInstantiate(assets->sphere, gameState->barnWallCleanMat, transform(float3(4,4,4), float3(3, 3, 3)));
    
    StaticMeshInstantiate(assets->skydome, gameState->skydomeMat, transform(float3(0, 0, 0), float3(80, 80, 80)));

}

void gryphkissUpdate()
{
    GameState* gameState = haven->gameState;

    // Entity Update
    HandUpdate(gameState->leftHand);
    HandUpdate(gameState->rightHand);
    
    PlayerUpdate(gameState->player);
    
    for (int i = 0; i < ArrayCount(haven->entities); i++)
    {
        Entity* entity = haven->entities[i];
        if (entity->type == EntityType_StaticMesh)
        {
            StaticMeshUpdate((StaticMesh*)entity, i);
        }
        if (entity->type == EntityType_ReflectionProbe)
        {
            ReflectionProbeUpdate((ReflectionProbe*)entity, i);
        }
    }
    
    LightBakerUpdate(gameState->lightBaker);
    
    Transform torvidTransform = {
        float3(2.5, 8, 0),
        input->playspaceStageLeft.right,
        input->playspaceStageLeft.forward,
        input->playspaceStageLeft.up,
        { 1, 1, 1 } };
    torvidTransform = rotate(torvidTransform, torvidTransform.up, -0.4);
    
    Animation* torvidAnimation = assets->torvidTestAnim;
    // Set up bones
    for (int i = 0; i < torvidAnimation->boneCount; i++)
    {
        int frameOffset = ((int)gameState->torvidFrame) * torvidAnimation->boneCount;
        Transform t = torvidAnimation->transforms[i + frameOffset];
        gameState->torvidMat->shaderBoneTransforms[i] = t;
    
        // Draw bone
        if (haven->editor)
        {
            t.scale = vectorOne * 0.05;
            t = LocalToWorld(t, torvidTransform);
            Drawing::DrawBox(t, 0.005);
            Bone* parentBone = assets->torvidTest->boneHierarchy[i].parent;
            if (parentBone)
            {
                float3 parentPos = torvidAnimation->transforms[parentBone->index + frameOffset].position;
                parentPos = LocalToWorld(parentPos, torvidTransform);
                Drawing::DrawLine(parentPos, t.position);
            }
        }
    }
    
    // Play animation
    float animationFPS = 25.0f;
    if (gameState->torvidFrame == 0.0f)
    {
        PlaySound(assets->Torvid_Gryphon, 0.25f);
    }
    gameState->torvidFrame += animationFPS * input->deltaTime;
    if (gameState->torvidFrame > 180.0f)
    {
        gameState->torvidFrame = 0.0f;
    }
    
    // Draw torvid
    Drawing::DrawMesh(gameState->torvidMat, assets->torvidTest, torvidTransform);


    // Gradient for testing color bit-depth
    //CreateMaterialLocal(gradientMateiral, assets->unlit, unlit);
    //gradientMateiral->ColorTexture = assets->pixelTest1;// assets->gradient;
    //gradientMateiral->Color = float3(1, 1, 1);
    //DrawMesh(gradientMateiral, assets->ui_quad, transform(float3(2, 3, 0.1)));



    input->vibrationAmplitudeRight = input->grabRight;


    input->handRight.scale = { 0.1f, 0.1f, 0.1f };
    input->handLeft.scale = { 0.1f, 0.1f, 0.1f };
    input->aimRight.scale = { 0.1f, 0.1f, 0.1f };
    input->aimLeft.scale = { 0.1f, 0.1f, 0.1f };

    float3 center = float3(0, 0, 0);// (input->playspaceStageLeft.position + input->playspaceStageRight.position) / 2.0f;
    Transform monkeyRotation = { center +
        float3(-1, 0, 0),
        input->playspaceStageLeft.right,
        input->playspaceStageLeft.forward,
        input->playspaceStageLeft.up,
        { 1, 1, 1 } };
    monkeyRotation = rotate(monkeyRotation, 0, 0, 0.0f);


    // Simulation plane
    CreateMaterialLocal(waterPlane, assets->unlit, unlit);
    waterPlane->ColorTexture = haven->waterRipplesCurrent;
    waterPlane->Color = float3(1.0f, 1.0f, 1.0f);
    waterPlane->mesh = assets->ui_quad;
    waterPlane->BackFaceCulling = true;
    Drawing::DrawMesh(waterPlane, assets->ui_quad, transform(float3(0, 0, 0.1)));


    //CreateMaterialLocal(fontTest, assets->unlit, unlit);
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

    char* my_str = R"(


Hello.
This is a demo program running natively on Quest 2, Written 
from scratch in C/C++. Right now it's just an empty-ish scene. 
I am slowly adding features and building it out.

Check out #6c84daff#torvid.net#ffffff# to see my other projects. :>

Controls: 
 - **Walk**: Left thumbstick.
 - **Rotate**: Right thumbstick.

Key systems so far:
 - Skeletal Animation.
 - Baked lighting using Spherical Harmonics.
 - Text Rendering.
 - Custom 3D model and animation format.
 - Scene with #ff0000ff#X-right#ffffffff#, #00ff00ff#Y-forward#ffffffff# and #0000ffff#Z-up#ffffffff# (as god intended).
)";

    //
    //Key APIs, Librariesand tools used :
    //-OpenXR from VR input.
    //    - OpenGL - ES for graphics.
    //    - OpenSL - ES for sound.
    //    - ASTC texture compression.
    //    - "Atkinson Hyperlegible" Font.

    Drawing::DrawFont2D("@", input->mousePos, 500, 600, haven->hAlign, haven->vAlign);
    //
    //DrawFont2D(lorem4, float2(400, 400), 300, 700, haven->hAlign, haven->vAlign);
    //DrawFontCameraFacing(lorem4, center + float3(0, 0, 3), 0.75, 2.0f * 0.75f, haven->hAlign, haven->vAlign);
    Drawing::DrawFont(my_str, transform(center + float3(5 - 0.1, 4, 2.5), 0, 0, -0.25), 0.8, 8.0f, HAlign_right, VAlign_down);

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
}
