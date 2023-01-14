#pragma once

#include "haven.cpp"

enum EntityType
{
    EntityType_Hand,
    EntityType_Player,
    EntityType_StaticMesh,
    EntityType_LightBaker,
};

#include "entities/hand.cpp"
#include "entities/player.cpp"
#include "entities/staticMesh.cpp"
#include "entities/lightBaker.cpp"

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

    Hand* leftHand;
    Hand* rightHand;

    Player* player;
    LightBaker* lightBaker;
};


void gryphkissStart()
{
    haven->gameState = ArenaPushStruct(&haven->arenaEngineState, GameState, "GameState");
    GameState* gameState = haven->gameState;

    ArenaInitialize(&gameState->arenaScene, Megabytes(64), (uint8*)ArenaPushBytes(&haven->arenasArena, Megabytes(64), "Scene", true), "Scene");

    gameState->lightBaker = LightBakerStart();

    // Load Torvid
    CreateMaterialGlobal(gameState->torvidMat, assets->defaultlit, Material_defaultlit);
    gameState->torvidMat->texM1 = assets->TorvidM1;
    gameState->torvidMat->texM2 = assets->TorvidM2;
    gameState->torvidMat->BackFaceCulling = true;

    // Create materials and load textures
    CreateMaterialGlobal(gameState->barnWallMat, assets->defaultlit, Material_defaultlit);
    gameState->barnWallMat->texM1 = assets->BarnWallM1;
    gameState->barnWallMat->texM2 = assets->BarnWallM2;
    CreateMaterialGlobal(gameState->barnWallCleanMat, assets->defaultlit, Material_defaultlit);
    gameState->barnWallCleanMat->texM1 = assets->BarnWallCleanM1;
    gameState->barnWallCleanMat->texM2 = assets->BarnWallCleanM2;
    CreateMaterialGlobal(gameState->barnCeilingMat, assets->defaultlit, Material_defaultlit);
    gameState->barnCeilingMat->texM1 = assets->BarnCeilingM1;
    gameState->barnCeilingMat->texM2 = assets->BarnCeilingM2;
    CreateMaterialGlobal(gameState->barnTilesMat, assets->defaultlit, Material_defaultlit);
    gameState->barnTilesMat->texM1 = assets->TilesM1;
    gameState->barnTilesMat->texM2 = assets->TilesM2;
    CreateMaterialGlobal(gameState->StrawPileMat, assets->defaultlit, Material_defaultlit);
    gameState->StrawPileMat->texM1 = assets->StrawPileM1;
    gameState->StrawPileMat->texM2 = assets->StrawPileM2;

    gameState->leftHand  = Instantiate(Hand);
    gameState->rightHand = Instantiate(Hand);
    gameState->leftHand->handSide = HandSide_Left;
    gameState->rightHand->handSide = HandSide_Right;
    HandStart(gameState->leftHand);
    HandStart(gameState->rightHand);

    gameState->player = Instantiate(Player);
    PlayerStart(gameState->player);

    float3 center = float3(0, 0, 0);// (input->playspaceStageLeft.position + input->playspaceStageRight.position) / 2.0f;
    StaticMeshInstantiate(assets->tonk, gameState->barnWallMat, transform(float3(3, 5, 0), float3(0.1, 0.2, 0.1)));
    //center += float3(-3, -5, 0);
    // left wall
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            if (y == 1)
            {
                StaticMeshInstantiate(assets->BarnWall02, gameState->barnWallCleanMat, transform(center + float3(0, x * 2, y), 0, 0, -0.25));
            }
            else
            {
                StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(center + float3(0, x * 2, y), 0, 0, -0.25));
            }
        }
    }

    
    // Draw barn?
    //DrawMesh(gameState->barnWallCleanMat, gameState->barnCeiling, transformIdentity);

    // left wall
    //for (int x = 0; x < 5; x++)
    //{
    //    for (int y = 0; y < 3; y++)
    //    {
    //        if (y == 1)
    //        {
    //            //&& x % 2 == 0
    //            StaticMeshInstantiate(assets->BarnWall02, gameState->barnWallCleanMat, transform(center + float3(0, x * 2, y), 0, 0, -0.25));
    //        }
    //        else
    //        {
    //            StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(center + float3(0, x * 2, y), 0, 0, -0.25));
    //        }
    //    }
    //}


    // right wall
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(center + float3(5, x * 2 + 2, y), 0, 0, 0.25));
        }
    }

    // floor
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            StaticMeshInstantiate(assets->ui_quad, gameState->barnTilesMat, transform(center + float3(x, y, 0)*2, float3(2,2,2)));
        }
    }

    // front wall
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(center + float3(x*2+2, 0, y+2), 0, 0, 0.0));
        }
    }
    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(center + float3(3, 0, 4), 0, 0, 0.0));
    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(center + float3(5, 0, 4), 0, 0, 0.0));
    StaticMeshInstantiate(assets->BarnWall02, gameState->barnWallCleanMat, transform(center + float3(1, 0, 0), 0, 0, 0.0));
    StaticMeshInstantiate(assets->BarnWall02, gameState->barnWallCleanMat, transform(center + float3(1, 0, 1), 0, 0, 0.0));

    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(center + float3(5, 0, 0), 0, 0, 0.0));
    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(center + float3(5, 0, 1), 0, 0, 0.0));

    // doors
    StaticMeshInstantiate(assets->BarnDoor, gameState->barnWallCleanMat, transform(center + float3(1, 0, 2), 0, 0, 0.0, float3(-1, 1, -1)));
    StaticMeshInstantiate(assets->BarnDoor, gameState->barnWallCleanMat, transform(center + float3(3, 0, 0), 0, 0, 0.0));


    // back wall
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(center + float3(x * 2 , 10, y), 0, 0, 0.5));
        }
    }
    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(center + float3(1, 10, 4), 0, 0, 0.5));
    StaticMeshInstantiate(assets->BarnWall01, gameState->barnWallCleanMat, transform(center + float3(3, 10, 4), 0, 0, 0.5));

    // ceiling left
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            StaticMeshInstantiate(assets->BarnCeiling01, gameState->barnWallCleanMat, transform(center + float3(x*0.704f, y*2, x * 0.704f) + float3(0, 2, 3), 0, 0.125, 0.25));
        }
    }

    // ceiling right
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            StaticMeshInstantiate(assets->BarnCeiling01, gameState->barnWallCleanMat, transform(center + float3(-x * 0.704f, y * 2, x * 0.704f) + float3(5, 0, 3), 0, -0.125, -0.25));
        }
    }
    
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
        if (haven->entities[i]->type == EntityType_StaticMesh)
        {
            StaticMeshUpdate((StaticMesh*)haven->entities[i], i);
        }
    }

    LightBakerUpdate(gameState->lightBaker);

    // Set up bones
    for (int i = 0; i < assets->torvidTestAnim->boneCount; i++)
    {
        int frameOffset = ((int)gameState->torvidFrame) * assets->torvidTestAnim->boneCount;
        Transform t = assets->torvidTestAnim->transforms[i + frameOffset];
        gameState->torvidMat->shaderBoneTransforms[i] = t;

        
        //DrawBox(LocalToWorld(t,), 0.005);
        //// Draw bone
        //if (!engineState->torvidTest->boneHierarchy[i].parent)
        //    continue;
        //float3 parentPos = engineState->torvidTestAnimation->transforms[parentIndex + frameOffset].position;

    }
    //// draw torvid bones
    //for (int i = 0; i < assets->torvidTestAnimation->boneCount; i++)
    //{
    //    int frameOffset = ((int)engineState->torvidFrame) * engineState->torvidTestAnimation->boneCount;
    //    Transform t = engineState->torvidTestAnimation->transforms[i + frameOffset];
    //    float distanceToParent = 0.5f;
    //    float boneRadius = 0.03f;
    //    t.scale = vectorOne * boneRadius;
    //    DrawMesh(memory, engineState, gameState->boneMaterial, engineState->boneSphere, ApplyTransform(t, monkeyRotation));
    //    if (!engineState->torvidTest->boneHierarchy[i].parent)
    //        continue;
    //
    //    Transform t2 = t;
    //    int parentIndex = engineState->torvidTest->boneHierarchy[i].parent->index;
    //    float3 parentPos = engineState->torvidTestAnimation->transforms[parentIndex + frameOffset].position;
    //    distanceToParent = distance(parentPos, t2.position);
    //    t2.forward = normalize(t2.position - parentPos);
    //    t2.right = normalize(cross(t2.forward, t2.up));
    //    t2.up = normalize(cross(t2.forward, t2.right));
    //    t2.position = parentPos + t2.forward * boneRadius;
    //    t2.scale = float3(boneRadius, distanceToParent - boneRadius * 2, boneRadius);
    //
    //    DrawMesh(memory, engineState, gameState->boneMaterial, engineState->bonePyramid, ApplyTransform(t2, monkeyRotation));
    //}

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
    float3 center = float3(0, 0, 0);// (input->playspaceStageLeft.position + input->playspaceStageRight.position) / 2.0f;

    //DrawMesh(gameState->barnWallMat, assets->tonk, transform(center, float3(0.1, 0.2, 0.1)));

    //center += float3(-3, -5, 0);
    Transform torvidPos = { center +
        float3(2.5, 8, 0),
        input->playspaceStageLeft.right,
        input->playspaceStageLeft.forward,
        input->playspaceStageLeft.up,
        { 1, 1, 1 } };

    //torvidPos = haven->textTransform;
    //torvidPos.position += -torvidPos.up * 0.9;
    //torvidPos.position += torvidPos.right * 1.3;
    //torvidPos.position += -torvidPos.forward * 1.5;
    //torvidPos = rotate(monkeyRotation, monkeyRotation.up, -45);
    torvidPos = rotate(torvidPos, torvidPos.up, -0.4);
    
    DrawMesh(gameState->torvidMat, assets->torvidTest, torvidPos);

}
