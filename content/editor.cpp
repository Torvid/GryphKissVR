#pragma once

#include "haven.cpp"

void editorStart()
{

}

void UpdateEditorCamera()
{
    float mouseSensitivity = 0.1f;

    if (!input->mouseRight)
        return;

    float mouseDeltaX = input->mousePosDelta.x;
    float mouseDeltaY = input->mousePosDelta.y;
    if (haven->spectatorCamera.up.z < 0)
    {
        mouseDeltaX = -mouseDeltaX;
    }
    haven->spectatorCamera = rotate(haven->spectatorCamera, float3(0, 0, 1), -mouseDeltaX * 0.005f * mouseSensitivity);
    haven->spectatorCamera = rotate(haven->spectatorCamera, haven->spectatorCamera.right, -mouseDeltaY * 0.005f * mouseSensitivity);

    int x = input->w ? 1 : 0 + input->s ? -1 : 0;
    int y = input->d ? 1 : 0 + input->a ? -1 : 0;
    int z = input->e ? 1 : 0 + input->q ? -1 : 0;

    float3 Movement = float3(0, 0, 0);
    Movement += haven->spectatorCamera.forward * x;
    Movement += haven->spectatorCamera.right * y;
    Movement += haven->spectatorCamera.up * z;

    if (length(Movement) > 0)
        Movement = normalize(Movement);

    float speed = 4;
    if (input->shift)
        speed = 16;

    haven->spectatorCamera.position += Movement * input->deltaTime * speed;
    //haven->spectatorCamera = input->head;

}


void editorUpdate()
{
    UpdateEditorCamera();

    if (haven->editor)
    {
        if (input->ctrl && input->sDown)
        {
            //memory->printf("Save\n");
            haven->platformWriteFile((uint8*)haven, 2048, "scene.cpp");
        }
        if (DrawButton("Load"))
        {

        }

        DrawMesh(haven->axesMaterial, assets->axes, input->handRight);
        DrawMesh(haven->axesMaterial, assets->axes, input->handLeft);
        DrawMesh(haven->axesMaterial, assets->axes, input->aimRight);
        DrawMesh(haven->axesMaterial, assets->axes, input->aimLeft);
        DrawMesh(haven->axesMaterial, assets->axes, input->playspaceStage);
        DrawMesh(haven->axesMaterial, assets->axes, input->playspaceStageLeft);
        DrawMesh(haven->axesMaterial, assets->axes, input->playspaceStageRight);
        DrawMesh(haven->axesMaterial, assets->axes, input->playspaceStageLeftRotated);
        DrawMesh(haven->axesMaterial, assets->axes, input->playspaceStageRightRotated);

        DrawText3D("World Origin", vectorOne * 0.1f);
        DrawText3D("Stage", input->playspaceStage.position + vectorOne * 0.1f);
        DrawText3D("Stage Left", input->playspaceStageLeft.position + vectorOne * 0.1f);
        DrawText3D("Stage Right", input->playspaceStageRight.position + vectorOne * 0.1f);

        DrawText3D("Left Hand", input->handLeft.position + 
            (input->handLeft.forward + 
             input->handLeft.right + 
             input->handLeft.up) * 0.05f, 0.1f);
        DrawText3D("Right Hand", input->handRight.position +
            (input->handRight.forward +
             input->handRight.right +
             input->handRight.up) * 0.05f, 0.1f);
        DrawText3D("Left Aim", input->aimLeft.position +
            (input->aimLeft.forward +
             input->aimLeft.right +
             input->aimLeft.up) * 0.05f, 0.1f);
        DrawText3D("Right Aim", input->aimRight.position +
            (input->aimRight.forward +
             input->aimRight.right +
             input->aimRight.up) * 0.05f, 0.1f);


    }


    if (haven->editor)
    {
        float3 center = (input->playspaceStageLeft.position + input->playspaceStageRight.position) / 2.0f;

        DrawMesh(haven->red, assets->box, { float3(0,0,0) + float3(0.25,   0,    0),    { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 0.5,  0.025, 0.025 } });
        DrawMesh(haven->green, assets->box, { float3(0,0,0) + float3(0,    0.25, 0),    { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 0.025, 0.5,  0.025 } });
        DrawMesh(haven->blue, assets->box, { float3(0,0,0) + float3(0,     0,    0.25), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 0.025, 0.025, 0.5  } });

        //Transform monkeyRotation2 = { center + float3(0, -6, 0), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 } };
        //DrawMesh(gameState->axesMaterial, haven->monkey, monkeyRotation2);
        //monkeyRotation2 = { center + float3(3, -3, 0), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 } };
        //DrawMesh(gameState->axesMaterial, haven->ui_quad, monkeyRotation2);
        //monkeyRotation2 = { center + float3(-3, -3, 0), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 } };
        //DrawMesh(gameState->torvidMat, haven->monkey, monkeyRotation2);
    }

    if (haven->editor)
    {
        const int tempStringSize = 2048;
        char text[tempStringSize] = {};
        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "\n\nINPUT: ");
        StringAppend(text, "\n    Mouse pos: ", input->mousePos);
        StringAppend(text, "\n    Spectator pos: ", haven->spectatorCamera.position);
        DrawText(text);

        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "    Left Controller:\n");
        StringAppend(text, "        Pos: ", input->handLeft.position);
        StringAppend(text, "\n        Trigger: ", input->triggerLeft);
        StringAppend(text, "\n        Thumbstick: ", input->thumbstickLeft);
        StringAppend(text, "\n        Face Button: ", input->faceButtonLeft);
        DrawText(text);

        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "    Right Controller:\n");
        StringAppend(text, "        Pos: ", input->handRight.position);
        StringAppend(text, "\n        Trigger: ", input->triggerRight);
        StringAppend(text, "\n        Thumbstick: ", input->thumbstickRight);
        StringAppend(text, "\n        Face Button: ", input->faceButtonRight);
        DrawText(text);

        //haven->headsetView = DrawToggle(input, "Headset View: ", haven->headsetView);

        if (DrawButton("Play a sound") || input->faceButtonLeftDown)
        {
            PlaySound(assets->HitWoodDome_22k_mono, 1.0f, false);
        }

        Transform testTransform = transform(float3(12, 0, 0), 0.1, 0.2, 0.3, float3(1, 0.75, 0.5) * 0.75);

        DrawText3D("Hello Debug Text\novo", float3(4, 0, 0), 2.0f);
        DrawLine(float3(4, 0, 0), float3(5, 1, 1), 0.05f, float3(0.6,0.1,0.2));
        DrawCircle(float3(6, 0, 0), float3(0, 0, 1), 0.75, 0.05f);
        DrawSphere(float3(8, 0, 0), 0.75, 0.05f);
        DrawAxisSphere(float3(10, 0, 0), 0.75, 0.05f);
        DrawTransform(testTransform, 0.05f);
        DrawPoint(float3(14, 0, 0), 0.25f);
        DrawAABB(float3(16, 0, 0), float3(1, 0.75, 0.5)* 0.75, 0.05f);
        DrawBox(testTransform, 0.05f);
        DrawBox2D(float2(200, 100), float2(100, 100), float3(0.5f, 0.0f, 0.0f), 0.5f);
        DrawBox2D(float2(250, 150), float2(100, 100), float3(0.0f, 0.5f, 0.0f), 0.5f);
        DrawBox2D(float2(300, 200), float2(100, 100), float3(0.0f, 0.0f, 0.5f), 0.5f);


        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "\n\nSOUND: \n");
        for (int i = 0; i < ArrayCapacity(haven->soundChannels); i++)
        {
            SoundChannel* channel = haven->soundChannels[i];

            if (channel->playing)
            {
                StringAppend(text, "    ", (int)i, ", ");
                StringAppend(text, channel->sound->filename, ", ");
                StringAppend(text, ((float)channel->currentSample / (float)channel->sound->sampleCount), "\n");
            }
        }
        DrawText(text);
    }
}
