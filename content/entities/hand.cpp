#pragma once
#include "../haven.cpp"

enum HandSide
{
    HandSide_Left,
    HandSide_Right,
};

struct Hand
{
    EntityContents;
    int clickCount;
    Material_defaultlit* handMaterial;
    HandSide handSide;
};

void HandStart(Hand* hand)
{
    hand->clickCount = 0;
    CreateMaterialGlobal(hand->handMaterial, assets->defaultlit, Material_defaultlit);
}

void HandUpdate(Hand* hand)
{
    if (hand->handSide == HandSide_Left)
    {
        hand->transform = LocalToWorld(input->aimLeft, input->playspace);
        if (input->faceButtonLeftDown)
            hand->clickCount++;
    }
    if (hand->handSide == HandSide_Right)
    {
        hand->transform = LocalToWorld(input->aimRight, input->playspace);
        if (input->faceButtonRightDown)
            hand->clickCount++;
    }

    DrawRay(hand->transform.position, hand->transform.forward, 999, 0.01f);
    char count = '0' + hand->clickCount % 10;
    DrawText3D(&count, hand->transform.position, 0.1f);
}
