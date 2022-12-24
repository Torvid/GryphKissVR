#pragma once

#include "haven.cpp"
#include "hand.cpp"
#include "player.cpp"
#include "staticMesh.cpp"

struct GameState
{
    MemoryArena arenaGameState;
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
};

void gryphkissStart()
{
    haven->gameState = ArenaPushStruct(&haven->arenaEngineState, GameState, "GameState");
    GameState* gameState = haven->gameState;

    ArenaInitialize(&gameState->arenaGameState, Megabytes(64), (uint8*)ArenaPushBytes(&haven->arenasArena, Megabytes(64), "Game State", true), "Game State");
    ArenaInitialize(&gameState->arenaScene, Megabytes(64), (uint8*)ArenaPushBytes(&haven->arenasArena, Megabytes(64), "Scene", true), "Scene");

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

    //gameState->leftHand = HandInstantiate();
    //gameState->rightHand = HandInstantiate();

    gameState->leftHand  = Instantiate(Hand);
    gameState->rightHand = Instantiate(Hand);
    gameState->leftHand->handSide = HandSide_Left;
    gameState->rightHand->handSide = HandSide_Right;
    HandStart(gameState->leftHand);
    HandStart(gameState->rightHand);

    gameState->player = Instantiate(Player);
    PlayerStart(gameState->player);
}

void gryphkissUpdate()
{
    GameState* gameState = haven->gameState;

    // Entity Update
    HandUpdate(gameState->leftHand);
    HandUpdate(gameState->rightHand);

    PlayerUpdate(gameState->player);

    //for (int i = 0; i < ArrayCount(haven->entities); i++)
    //{
    //    haven->entities[i]->entityUpdate(haven->entities[i]);
    //}

    // Set up bones
    for (int i = 0; i < assets->torvidTestAnim->boneCount; i++)
    {
        int frameOffset = ((int)gameState->torvidFrame) * assets->torvidTestAnim->boneCount;
        Transform t = assets->torvidTestAnim->transforms[i + frameOffset];
        // TODO: fix this
        gameState->torvidMat->shaderBoneTransforms[i] = t;
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
    float3 center = (input->playspaceStageLeft.position + input->playspaceStageRight.position) / 2.0f;

    DrawMesh(gameState->barnWallMat, assets->tonk, transform(center, float3(0.1, 0.2, 0.1)));


    center += float3(-3, -5, 0);
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



    // Draw barn?
    //DrawMesh(gameState->barnWallCleanMat, gameState->barnCeiling, transformIdentity);

    // left wall
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            if (y == 1)
            {
                //&& x % 2 == 0
                DrawMesh(gameState->barnWallCleanMat, assets->BarnWall02, transform(center + float3(0, x * 2, y), 0, 0, -0.25));
            }
            else
            {
                DrawMesh(gameState->barnWallCleanMat, assets->BarnWall01, transform(center + float3(0, x * 2, y), 0, 0, -0.25));
            }
        }
    }

    // right wall
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            DrawMesh(gameState->barnWallCleanMat, assets->BarnWall01, transform(center + float3(5, x * 2 + 2, y), 0, 0, 0.25));
        }
    }

    // floor
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            DrawMesh(gameState->barnTilesMat, assets->ui_quad, transform(center + float3(x, y, 0)*2, float3(2,2,2)));
        }
    }

    // front wall
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            DrawMesh(gameState->barnWallCleanMat, assets->BarnWall01, transform(center + float3(x*2+2, 0, y+2), 0, 0, 0.0));
        }
    }
    DrawMesh(gameState->barnWallCleanMat, assets->BarnWall01, transform(center + float3(3, 0, 4), 0, 0, 0.0));
    DrawMesh(gameState->barnWallCleanMat, assets->BarnWall01, transform(center + float3(5, 0, 4), 0, 0, 0.0));
    DrawMesh(gameState->barnWallCleanMat, assets->BarnWall02, transform(center + float3(1, 0, 0), 0, 0, 0.0));
    DrawMesh(gameState->barnWallCleanMat, assets->BarnWall02, transform(center + float3(1, 0, 1), 0, 0, 0.0));

    DrawMesh(gameState->barnWallCleanMat, assets->BarnWall01, transform(center + float3(5, 0, 0), 0, 0, 0.0));
    DrawMesh(gameState->barnWallCleanMat, assets->BarnWall01, transform(center + float3(5, 0, 1), 0, 0, 0.0));

    // doors
    DrawMesh(gameState->barnWallCleanMat, assets->BarnDoor, transform(center + float3(1, 0, 2), 0, 0, 0.0, float3(-1, 1, -1)));
    DrawMesh(gameState->barnWallCleanMat, assets->BarnDoor, transform(center + float3(3, 0, 0), 0, 0, 0.0));


    // back wall
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            DrawMesh(gameState->barnWallCleanMat, assets->BarnWall01, transform(center + float3(x * 2 , 10, y), 0, 0, 0.5));
        }
    }
    DrawMesh(gameState->barnWallCleanMat, assets->BarnWall01, transform(center + float3(1, 10, 4), 0, 0, 0.5));
    DrawMesh(gameState->barnWallCleanMat, assets->BarnWall01, transform(center + float3(3, 10, 4), 0, 0, 0.5));

    // ceiling left
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            DrawMesh(gameState->barnWallCleanMat, assets->BarnCeiling01, transform(center + float3(x*0.704f, y*2, x * 0.704f) + float3(0, 2, 3), 0, 0.125, 0.25));
        }
    }

    // ceiling right
    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            DrawMesh(gameState->barnWallCleanMat, assets->BarnCeiling01, transform(center + float3(-x * 0.704f, y * 2, x * 0.704f) + float3(5, 0, 3), 0, -0.125, -0.25));
        }
    }

}
