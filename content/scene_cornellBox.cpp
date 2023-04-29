#pragma once

#include "haven.cpp"

namespace CornellBox
{
    struct GameState
    {
        Material_defaultlit* cornellBoxMat;
        Material_defaultlit* boxMat;
        LightBaker* lightBaker;
        Material_defaultlit* torvidMat;
        ReflectionProbe* probe0;
    };

    void Start()
    {
        haven->gameState = ArenaPushStruct(&haven->arenaScene, GameState, "GameState");
        GameState* gameState = (GameState*)haven->gameState;

        CreateSceneMaterial(gameState->cornellBoxMat);
        gameState->cornellBoxMat->texM1 = assets->cornellBoxM1;
        gameState->cornellBoxMat->texM2 = assets->baseM2;
        gameState->cornellBoxMat->texM3 = assets->cornellBoxM3;

        CreateSceneMaterial(gameState->boxMat);
        gameState->boxMat->texM1 = assets->baseM1;
        gameState->boxMat->texM2 = assets->baseM2;
        gameState->boxMat->texM3 = assets->baseM3;

        CreateSceneMaterial(gameState->torvidMat);
        gameState->torvidMat->texM1 = assets->TorvidM1;
        gameState->torvidMat->texM2 = assets->TorvidM2;
        gameState->torvidMat->texM3 = assets->TorvidM3;

        //gameState->boxMat->BackFaceCulling = false;
        StaticMeshInstantiate(assets->cornellBox, gameState->cornellBoxMat, transform(float3(0, 0, 0), float3(1, 1, 1)));
        gameState->lightBaker = LightBakerInstantiate(assets->bake_CornellBox, "bake_CornellBox.rad", float3(-6, -6, -1), float3(12, 12, 12));
    
        StaticMeshInstantiate(assets->box, gameState->boxMat, rotate(transform(float3(-2, -2, 2), float3(2.5, 2.5, 6)), 0, 0.01, 0.05));

        Transform sphere = transform();
        sphere.position = float3(2, 2, 2);
        sphere.scale = vectorOne*1.2;
        StaticMeshInstantiate(assets->sphere, gameState->boxMat, sphere);

        gameState->probe0 = ReflectionProbeInstantiate(transform(float3(0, 0, 5)));
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
        Animation* torvidAnimation = assets->torvidTestAnim;
        for (int i = 0; i < torvidAnimation->boneCount; i++)
        {
            int frameOffset = 0;// ((int)gameState->torvidFrame)* torvidAnimation->boneCount;
            Transform t = torvidAnimation->transforms[i + frameOffset];
            gameState->torvidMat->shaderBoneTransforms[i] = t;
        }
        Rendering::DrawMesh(gameState->torvidMat, assets->torvidTest, rotate(transform(float3(0.5f, sin(input->time) * 2.5f, 0)), 0,0,0.1));

        Drawing::DrawFont("the light ray knows where it is because it knows where it isn't", transform(float3(0, 0, 1), 0, 0, 0.25), 0.8, 8.0f, HAlign_right, VAlign_down);
    }
}
