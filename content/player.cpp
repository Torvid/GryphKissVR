#pragma once
#include "haven.cpp"

struct Player
{
    EntityContents;
};

void PlayerStart(Player* player)
{
    player->transform = transform(float3(0, 0, 0));
}

void PlayerUpdate(Player* player)
{
    //input->eyeLeft.position += input->eyeLeft.right;
    //input->eyeRight.position -= input->eyeRight.right;

    float moveSpeed = 2.0f;
    float snapTurnSteps = 12.0f;

    float2 delta = input->thumbstickLeft;
    delta = MoveTowards(delta, float2(0, 0), 0.35f); // deadzone
    
    input->head = LocalToWorld(input->playspace, input->head);

    player->transform.position += (input->head.right * float3(1, 1, 0)) * delta.x * input->deltaTime * moveSpeed;
    player->transform.position += (input->head.forward * float3(1, 1, 0)) * delta.y * input->deltaTime * moveSpeed;
    
    if (input->faceButtonLeftDown)
        player->transform = transform(float3(0, 0, 0));

    player->transform.position.z = 0;

    // rotate
    if (input->thumbstickRight.x < -0.5 && input->LastthumbstickRight.x > -0.5)
        player->transform = rotate(player->transform, 0, 0, 1.0f / snapTurnSteps);
    if (input->thumbstickRight.x > 0.5 && input->LastthumbstickRight.x < 0.5)
        player->transform = rotate(player->transform, 0, 0, -(1.0f / snapTurnSteps));

    // move the playspace around
    input->playspace = player->transform;
}
