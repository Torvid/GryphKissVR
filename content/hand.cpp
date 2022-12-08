#pragma once
#include "haven.cpp"

struct Hand
{
    EntityContents;
    int clickCount;
    Material_defaultlit* handMaterial;
};


void HandStart(Entity* entity, Input* input)
{
    Hand* hand = (Hand*)entity;

    hand->clickCount = 0;
    CreateMaterialGlobal(hand->handMaterial, assets->defaultlit, Material_defaultlit);
}

void HandUpdate(Entity* entity, Input* input)
{
    Hand* hand = (Hand*)entity;
    
    if (input->aDown)
    {
        hand->clickCount++;
    }
    
    hand->transform = input->handLeft;
    hand->transform.scale = float3(0.01, 0.01, 0.01);
    DrawText3D("aaaa", hand->transform.position, 0.1f);
    DrawMesh(hand->handMaterial, assets->tonk, hand->transform);
}

Hand* HandInstantiate(Input* input)
{
    Hand* newHand = ArenaPushStruct(&haven->arenaScene, Hand, "hand");
    ArrayAdd(haven->entities, (Entity*)newHand);
    newHand->entityStart = &HandStart;
    newHand->entityUpdate = &HandUpdate;
    newHand->entityStart((Entity*)newHand, input);

    return newHand;
}
