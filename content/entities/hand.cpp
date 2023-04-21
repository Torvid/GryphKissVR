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
    CreateMaterialGlobal(self->handMaterial, assets->defaultlit, defaultlit);
    ArrayAdd(haven->sceneMaterials, self->handMaterial);
    self->handMaterial->texM1 = assets->BarnWallM1;
    self->handMaterial->texM2 = assets->BarnWallM2;
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

    Drawing::DrawRay(self->transform.position, self->transform.forward, 999, 0.01f);
    char text[2];
    text[0] = '0' + self->clickCount % 10;
    text[1] = 0;
    //DrawText3D(&count, self->transform.position, 0.2f);
    Drawing::DrawFontCameraFacing(text, self->transform.position + float3(0,0,0.1), 0.8, 8.0f, HAlign_center, VAlign_center);

    Transform tonkTransform = self->transform;
    tonkTransform.scale = float3(0.01, 0.01, 0.01);
    self->tonk->transform = tonkTransform;
    //DrawMesh(self->handMaterial, assets->tonk, tonkTransform);
}
