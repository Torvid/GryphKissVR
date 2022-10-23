#pragma once

#include "haven.cpp"

struct GameState
{
    // Torvid
    float torvidFrame;
    Material_defaultlit* torvidMat;
    Mesh* torvidTest;
    Animation* torvidTestAnimation;
    Sound* torvidSound;


    Material_unlit* axesMaterial;
    Material_unlit* boneMaterial;
    Material_unlit* red;
    Material_unlit* green;
    Material_unlit* blue;

    // Barn stuff
    Material_defaultlit* barnWallMat;
    Material_defaultlit* barnWallCleanMat;
    Material_defaultlit* barnCeilingMat;
    Material_defaultlit* barnTilesMat;
    Material_defaultlit* StrawPileMat;

    Mesh* barnWall01;
    Mesh* barnWall02;
    Mesh* barnWall03;
    Mesh* barnCeiling;
    Mesh* BarnBeam01;
    Mesh* BarnBeam02;
    Mesh* BarnDoor;
    Mesh* StrawPile;

};

void gryphkissStart(EngineState* engineState, GameState* gameState, Input* input)
{
    // Load Torvid
    CreateMaterialGlobal(engineState, gameState->torvidMat, engineState->defaultlit, Material_defaultlit);
    gameState->torvidMat->texM1 = FileReadTexture(engineState, "TorvidM1.tga");
    gameState->torvidMat->texM2 = FileReadTexture(engineState, "TorvidM2.tga");
    gameState->torvidMat->BackFaceCulling = true;

    gameState->torvidTest = FileReadMesh(engineState, "torvidTest.mesh");
    gameState->torvidTestAnimation = FileReadAnimation(engineState, "torvidTest.anim");
    gameState->torvidSound = FileReadSound(engineState, "Torvid_Gryphon.wav");

    // Create materials and load textures
    CreateMaterialGlobal(engineState, gameState->barnWallMat, engineState->defaultlit, Material_defaultlit);
    gameState->barnWallMat->texM1 = FileReadTexture(engineState, "barn/BarnWallM1.tga");
    gameState->barnWallMat->texM2 = FileReadTexture(engineState, "barn/BarnWallM2.tga");
    CreateMaterialGlobal(engineState, gameState->barnWallCleanMat, engineState->defaultlit, Material_defaultlit);
    gameState->barnWallCleanMat->texM1 = FileReadTexture(engineState, "barn/barnWallCleanM1.tga");
    gameState->barnWallCleanMat->texM2 = FileReadTexture(engineState, "barn/barnWallCleanM2.tga");
    CreateMaterialGlobal(engineState, gameState->barnCeilingMat, engineState->defaultlit, Material_defaultlit);
    gameState->barnCeilingMat->texM1 = FileReadTexture(engineState, "barn/barnCeilingM1.tga");
    gameState->barnCeilingMat->texM2 = FileReadTexture(engineState, "barn/barnCeilingM2.tga");
    CreateMaterialGlobal(engineState, gameState->barnTilesMat, engineState->defaultlit, Material_defaultlit);
    gameState->barnTilesMat->texM1 = FileReadTexture(engineState, "barn/TilesM1.tga");
    gameState->barnTilesMat->texM2 = FileReadTexture(engineState, "barn/TilesM2.tga");
    CreateMaterialGlobal(engineState, gameState->StrawPileMat, engineState->defaultlit, Material_defaultlit);
    gameState->StrawPileMat->texM1 = FileReadTexture(engineState, "barn/StrawPileM1.tga");
    gameState->StrawPileMat->texM2 = FileReadTexture(engineState, "barn/StrawPileM2.tga");
    
    // Load meshes
    gameState->barnWall01 = FileReadMesh(engineState, "barn/barnWall01.obj");
    gameState->barnWall02 = FileReadMesh(engineState, "barn/barnWall02.obj");
    gameState->barnWall03 = FileReadMesh(engineState, "barn/barnWall03.obj");
    gameState->barnCeiling = FileReadMesh(engineState, "barn/barnCeiling.obj");
    gameState->BarnBeam01 = FileReadMesh(engineState, "barn/BarnBeam01.obj");
    gameState->BarnBeam02 = FileReadMesh(engineState, "barn/BarnBeam02.obj");
    gameState->BarnDoor = FileReadMesh(engineState, "barn/BarnDoor.obj");
    gameState->StrawPile = FileReadMesh(engineState, "barn/StrawPile.obj");


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

}

void gryphkissUpdate(EngineState* engineState, GameState* gameState, Input* input)
{
    // Set up bones
    for (int i = 0; i < gameState->torvidTestAnimation->boneCount; i++)
    {
        int frameOffset = ((int)gameState->torvidFrame) * gameState->torvidTestAnimation->boneCount;
        Transform t = gameState->torvidTestAnimation->transforms[i + frameOffset];
        // TODO: fix this
        gameState->torvidMat->shaderBoneTransforms[i] = t;
    }

    // Play animation
    float animationFPS = 25.0f;
    if (gameState->torvidFrame == 0.0f)
    {
        PlaySound(engineState, gameState->torvidSound, 0.25f);
    }
    gameState->torvidFrame += animationFPS * input->deltaTime;
    if (gameState->torvidFrame > 180.0f)
    {
        gameState->torvidFrame = 0.0f;
    }

    // Draw torvid
    float3 center = (input->playspaceStageLeft.position + input->playspaceStageRight.position) / 2.0f;
    Transform monkeyRotation = { center +
        float3(-1, 0, 0),
        input->playspaceStageLeft.right,
        input->playspaceStageLeft.forward,
        input->playspaceStageLeft.up,
        { 1, 1, 1 } };
    monkeyRotation = engineState->textTransform;
    monkeyRotation.position += -monkeyRotation.up * 0.9;
    monkeyRotation.position += monkeyRotation.right * 1.3;
    monkeyRotation.position += -monkeyRotation.forward * 1.5;
    monkeyRotation = rotate(monkeyRotation, monkeyRotation.up, -45);
    DrawMesh(engineState, gameState->torvidMat, gameState->torvidTest, monkeyRotation);


    // Draw barn?

    DrawMesh(engineState, gameState->barnWallCleanMat, gameState->barnCeiling, transformIdentity);

}
