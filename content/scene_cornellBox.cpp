#pragma once

#include "haven.cpp"

namespace CornellBox
{
    struct GameState
    {
        Material_defaultlit* boxMat;
        LightBaker* lightBaker;
    };

    void Start()
    {
        haven->gameState = ArenaPushStruct(&haven->arenaEngineState, GameState, "GameState");
        GameState* gameState = (GameState*)haven->gameState;

        CreateSceneMaterial(gameState->boxMat);
        gameState->boxMat->texM1 = assets->BarnWallCleanM1;
        gameState->boxMat->texM2 = assets->BarnWallCleanM2;

        StaticMeshInstantiate(assets->cornellBox, gameState->boxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
        gameState->lightBaker = LightBakerInstantiate();
    }

    void Update()
    {
        GameState* gameState = (GameState*)haven->gameState;

        LightBakerUpdate(gameState->lightBaker);

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

        Drawing::DrawFont("Conrell Box", transform(float3(0, 0, 0), 0, 0, -0.25), 0.8, 8.0f, HAlign_right, VAlign_down);
    }
}
