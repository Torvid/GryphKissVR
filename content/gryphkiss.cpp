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
    CreateMaterialGlobal(name, defaultlit); \
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

    CreateMaterialGlobal(gameState->skydomeMat, skydomeShader);
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
            DrawBox(t, 0.005);
            Bone* parentBone = assets->torvidTest->boneHierarchy[i].parent;
            if (parentBone)
            {
                float3 parentPos = torvidAnimation->transforms[parentBone->index + frameOffset].position;
                parentPos = LocalToWorld(parentPos, torvidTransform);
                DrawLine(parentPos, t.position);
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
    DrawMesh(gameState->torvidMat, assets->torvidTest, torvidTransform);
    //torvidTransform.position = float3(2, 0.8, 0);
    //DrawMesh(gameState->torvidMat, assets->torvidTest, torvidTransform);
}
