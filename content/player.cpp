#pragma once
#include "haven.cpp"

struct Player
{
    EntityContents;
    float3 playspaceOffset;
    float3 playspaceOffsetLast;
};

void PlayerStart(Player* player)
{
    player->transform = transform(float3(0, 0, 0));
    player->playspaceOffset = float3(0, 0, 0);
    player->playspaceOffsetLast = float3(0, 0, 0);
}

void PlayerUpdate(Player* player)
{
    // Add offsets to line it up with data from the API
    float3 center = (input->playspaceStageLeft.position + input->playspaceStageRight.position) / 2.0f;
    center += float3(-3, -5, 0);
    center = -center;
    player->playspaceOffset = center;
    float3 playspaceDelta = float3(0, 0, 0);
    if (player->playspaceOffset != player->playspaceOffsetLast)
    {
        playspaceDelta = player->playspaceOffset - player->playspaceOffsetLast;
        player->playspaceOffsetLast = player->playspaceOffset;
    }
    player->transform.position += playspaceDelta;

    //input->eyeLeft.position += input->eyeLeft.right;
    //input->eyeRight.position -= input->eyeRight.right;

    float floorHeight = input->playspaceStageLeft.position.z;

    float moveSpeed = 2.0f;
    float snapTurnSteps = 12.0f;

    float2 delta = input->thumbstickLeft;
    delta = MoveTowards(delta, float2(0, 0), 0.35f); // deadzone
    
    Transform headLocal = LocalToWorld(input->playspace, input->head);

    player->transform.position += (headLocal.right * float3(1, 1, 0)) * delta.x * input->deltaTime * moveSpeed;
    player->transform.position += (headLocal.forward * float3(1, 1, 0)) * delta.y * input->deltaTime * moveSpeed;
    
    if (input->faceButtonLeftDown)
        player->transform = transform(float3(0, 0, 0));

    //player->transform.position.z = 0;
    
    //float3 cnn = input->head.position;
    //DrawCircle(cnn, float3(0, 0, 1), 0.5, 0.02, float3(0.5, 1, 0.5));

    // rotate
    if (input->thumbstickRight.x < -0.5 && input->LastthumbstickRight.x > -0.5)
        player->transform = rotateAboutPoint(player->transform, headLocal.position, float3(0, 0, 1),   1.0f / snapTurnSteps);
    if (input->thumbstickRight.x > 0.5 && input->LastthumbstickRight.x < 0.5)
        player->transform = rotateAboutPoint(player->transform, headLocal.position, float3(0, 0, 1), -(1.0f / snapTurnSteps));

    Transform headbox = headLocal;
    headbox.scale = float3(0.2, 0.1, 0.1);
    DrawBox(headbox, 0.01f);
    DrawTransform(input->head);
    DrawTransform(headLocal);

    DrawCircle(input->head.position, float3(0, 0, 1), 0.25, 0.02, float3(0.5, 0.5, 1.0));
    DrawCircle(player->transform.position - float3(0, 0, 0.1), float3(0, 0, 1), 0.5, 0.02);

    // move the playspace around
    input->playspace = player->transform;// +center;

    //input->playspace.position += center;
    DrawCircle(input->playspace.position, float3(0, 0, 1), 0.5, 0.02, float3(1, 0.5, 0.5));
}
