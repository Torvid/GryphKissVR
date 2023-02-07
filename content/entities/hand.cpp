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
    StaticMesh* tonk;
};

void HandStart(Hand* self)
{
    self->clickCount = 0;
    CreateMaterialGlobal(self->handMaterial, defaultlit);
    ArrayAdd(haven->sceneMaterials, self->handMaterial);
    self->tonk = StaticMeshInstantiate(assets->tonk, self->handMaterial, transform(float3(0, 0, 0)));
    
}

void HandUpdate(Hand* self)
{
    if (self->handSide == HandSide_Left)
    {
        self->transform = LocalToWorld(input->aimLeft, input->playspace);
        if (input->faceButtonLeftDown)
            self->clickCount++;
    }
    if (self->handSide == HandSide_Right)
    {
        self->transform = LocalToWorld(input->aimRight, input->playspace);
        if (input->faceButtonRightDown)
            self->clickCount++;
    }

    DrawRay(self->transform.position, self->transform.forward, 999, 0.01f);
    char count = '0' + self->clickCount % 10;
    DrawText3D(&count, self->transform.position, 0.2f);

    Transform tonkTransform = self->transform;
    tonkTransform.scale = float3(0.01, 0.01, 0.01);
    self->tonk->transform = tonkTransform;
    //DrawMesh(self->handMaterial, assets->tonk, tonkTransform);
}
