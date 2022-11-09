#pragma once
#include "haven.cpp"

struct Hand
{
    EntityContents;
    int clickCount;
    Material_defaultlit* handMaterial;
};


void HandStart(EngineState* engineState, Entity* entity, Input* input)
{
    Hand* hand = (Hand*)entity;

    hand->clickCount = 0;
    CreateMaterialGlobal(engineState, hand->handMaterial, engineState->defaultlit, Material_defaultlit);
}

void HandUpdate(EngineState* engineState, Entity* entity, Input* input)
{
    Hand* hand = (Hand*)entity;
    
    if (input->aDown)
    {
        hand->clickCount++;
    }
    
    hand->transform = input->handLeft;
    hand->transform.scale = float3(0.01, 0.01, 0.01);
    DrawText3D(engineState, "aaaa", hand->transform.position, 0.1f);
    DrawMesh(engineState, hand->handMaterial, assets->tonk, hand->transform);
}

Hand* HandInstantiate(EngineState* engineState, Input* input)
{
    Hand* newHand = ArenaPushStruct(&engineState->arenaScene, Hand, "hand");
    ArrayAdd(engineState->entities, (Entity*)newHand);
    newHand->entityStart = &HandStart;
    newHand->entityUpdate = &HandUpdate;
    newHand->entityStart(engineState, (Entity*)newHand, input);

    return newHand;
}
