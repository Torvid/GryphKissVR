#pragma once

//#include "../haven_platform.h"
//#include "../haven.cpp"
//#include "../math.cpp"
//#include "../memory.cpp"
//#include "../string.cpp"


void RatSpinInitialize(GameMemory* memory, EngineState* engineState, RatData* ratData)
{
    ratData->model = FileReadMesh(engineState, "rat/rat.obj");
    ratData->texture = FileReadTexture(engineState, "rat/rat.tga");
    ratData->ratBuffer = CreateTextureTarget(memory, engineState, 1024, 1024);
    ratData->ratBufferCopyTo = CreateTextureTarget(memory, engineState, 1024, 1024);

}

void RatSpinUpdate(GameMemory* memory, EngineState* engineState, RatData* ratData)
{

    float t = memory->input.time;
    t = mod(t, 20.0f);
    t = t * t;
    SetRenderTarget(memory, ratData->ratBufferCopyTo);
    DrawClear(memory, { 0.0, 0.0, 0.0, 0.0 });
    SetRenderTarget(memory, ratData->ratBuffer);
    DrawClear(memory, { 0.0, 0.0, 0.0, 0.0 });
    SetRenderTarget(memory, 0);
    DrawClear(memory, { 0.0, 0.0, 0.0, 0.0 });
    float steps = 200;
    float mul = 1.0f / steps;
    for (int i = 0; i < steps; i++)
    {
        SetRenderTarget(memory, ratData->ratBuffer);

        DrawClear(memory, { 0.5, 0.5, 0.5, 0.0 });

        CreateMaterialLocal(ratMaterial);
        ratMaterial->shader = engineState->defaultlit;
        ratMaterial->mesh = ratData->model;
        ratMaterial->cameraTransform = Transform(float3(0,1,7), vectorForward, vectorUp, vectorOne);
        ratMaterial->customCameraTransform = true;
        //ratMaterial->blendMode = BlendMode_Add;

        ratMaterial->transform = rotate(transformIdentity, 0, t + (i / steps) * (t/200.0f), 0);
        SetTexture(ratMaterial, "texM1", ratData->texture);
        SetFloat3(ratMaterial, "color", float3(1.0f, 1.0f, 1.0f));
        DrawMesh(memory, ratMaterial);
        
        SetRenderTarget(memory, ratData->ratBufferCopyTo);
        CreateMaterialLocal(copyTexture);
        copyTexture->mesh = engineState->ui_quad;
        copyTexture->shader = engineState->textureCopyShader;
        copyTexture->blendMode = BlendMode_Add;
        SetTexture(copyTexture, "ColorTexture", ratData->ratBuffer);
        DrawMesh(memory, copyTexture);

    }

    SetRenderTarget(memory, 0);
    DrawClear(memory);
    
    CreateMaterialLocal(copyTexture);
    copyTexture->shader = engineState->textureCopyShader;
    copyTexture->mesh = engineState->ui_quad;
    SetTexture(copyTexture, "ColorTexture", ratData->ratBufferCopyTo);
    SetFloat3(copyTexture, "Color", float3(mul, mul, mul));
    DrawMesh(memory, copyTexture);

}
