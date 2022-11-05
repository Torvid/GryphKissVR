#pragma once

#include "haven.cpp"

void editorStart(EngineState* engineState, GameState* gameState, Input* input)
{

}

void editorUpdate(EngineState* engineState, GameState* gameState, Input* input)
{
    UpdateEditorCamera(engineState, input);

    if (engineState->editor)
    {
        if (input->ctrl && input->sDown)
        {
            //memory->printf("Save\n");
            engineState->platformWriteFile((uint8*)engineState, 2048, "scene.cpp");
        }
        if (DrawButton(engineState, input, "Load"))
        {

        }

        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->handRight);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->handLeft);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->aimRight);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->aimLeft);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->playspaceStage);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->playspaceStageLeft);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->playspaceStageRight);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->playspaceStageLeftRotated);
        DrawMesh(engineState, gameState->axesMaterial, engineState->axes, input->playspaceStageRightRotated);

        DrawText3D(engineState, "World Origin", vectorOne * 0.1f);
        DrawText3D(engineState, "Stage", input->playspaceStage.position + vectorOne * 0.1f);
        DrawText3D(engineState, "Stage Left", input->playspaceStageLeft.position + vectorOne * 0.1f);
        DrawText3D(engineState, "Stage Right", input->playspaceStageRight.position + vectorOne * 0.1f);

        DrawText3D(engineState, "Left Hand", input->handLeft.position + 
            (input->handLeft.forward + 
             input->handLeft.right + 
             input->handLeft.up) * 0.05f, 0.1f);
        DrawText3D(engineState, "Right Hand", input->handRight.position +
            (input->handRight.forward +
             input->handRight.right +
             input->handRight.up) * 0.05f, 0.1f);
        DrawText3D(engineState, "Left Aim", input->aimLeft.position +
            (input->aimLeft.forward +
             input->aimLeft.right +
             input->aimLeft.up) * 0.05f, 0.1f);
        DrawText3D(engineState, "Right Aim", input->aimRight.position +
            (input->aimRight.forward +
             input->aimRight.right +
             input->aimRight.up) * 0.05f, 0.1f);

        //DrawText3D(engineState, "Stage Left Rotated", input->playspaceStageLeftRotated.position);
        //DrawText3D(engineState, "Stage Right Rotated", input->playspaceStageRightRotated.position);

    }


    if (engineState->editor)
    {
        float3 center = (input->playspaceStageLeft.position + input->playspaceStageRight.position) / 2.0f;

        DrawMesh(engineState, gameState->red, engineState->box, { float3(0,0,0) + float3(0.25,   0,    0),    { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 0.5,  0.025, 0.025 } });
        DrawMesh(engineState, gameState->green, engineState->box, { float3(0,0,0) + float3(0,    0.25, 0),    { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 0.025, 0.5,  0.025 } });
        DrawMesh(engineState, gameState->blue, engineState->box, { float3(0,0,0) + float3(0,     0,    0.25), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 0.025, 0.025, 0.5  } });

        //Transform monkeyRotation2 = { center + float3(0, -6, 0), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 } };
        //DrawMesh(engineState, gameState->axesMaterial, engineState->monkey, monkeyRotation2);
        //monkeyRotation2 = { center + float3(3, -3, 0), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 } };
        //DrawMesh(engineState, gameState->axesMaterial, engineState->ui_quad, monkeyRotation2);
        //monkeyRotation2 = { center + float3(-3, -3, 0), { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 } };
        //DrawMesh(engineState, gameState->torvidMat, engineState->monkey, monkeyRotation2);
    }

    if (engineState->editor)
    {
        const int tempStringSize = 2048;
        char text[tempStringSize] = {};
        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "\n\nINPUT: ");
        StringAppend(text, "\n    Mouse pos: ", input->mousePos);
        StringAppend(text, "\n    Eye pos: ", input->eyeLeft.position);
        DrawText(engineState, text);

        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "    Left Controller:\n");
        StringAppend(text, "        Pos: ", input->handLeft.position);
        StringAppend(text, "\n        Trigger: ", input->triggerLeft);
        StringAppend(text, "\n        Thumbstick: ", input->thumbstickLeft);
        StringAppend(text, "\n        Face Button: ", input->faceButtonLeft);
        DrawText(engineState, text);

        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "    Right Controller:\n");
        StringAppend(text, "        Pos: ", input->handRight.position);
        StringAppend(text, "\n        Trigger: ", input->triggerRight);
        StringAppend(text, "\n        Thumbstick: ", input->thumbstickRight);
        StringAppend(text, "\n        Face Button: ", input->faceButtonRight);
        DrawText(engineState, text);

        engineState->headsetView = DrawToggle(engineState, input, "Headset View: ", engineState->headsetView);

        if (DrawButton(engineState, input, "Play a sound") || input->faceButtonLeftDown)
        {
            PlaySound(engineState, engineState->TestSound_22k_mono, 1.0f, false);
        }

        Transform testTransform = transform(float3(12, 0, 0), 0.1, 0.2, 0.3, float3(1, 0.75, 0.5) * 0.75);

        DrawLine(engineState, float3(4, 0, 0), float3(1, 1, 1), 0.05f, float3(0.6,0.1,0.2));
        DrawCircle(engineState, float3(6, 0, 0), float3(0, 0, 1), 0.75, 0.05f);
        DrawSphere(engineState, float3(8, 0, 0), 0.75, 0.05f);
        DrawAxisSphere(engineState, float3(10, 0, 0), 0.75, 0.05f);
        DrawTransform(engineState, testTransform, 0.05f);
        DrawPoint(engineState, float3(14, 0, 0), 0.25f);
        DrawAABB(engineState, float3(16, 0, 0), float3(1, 0.75, 0.5)* 0.75, 0.05f);
        DrawBox(engineState, testTransform, 0.05f);


        Clear((uint8*)text, tempStringSize);
        StringAppend(text, "\n\nSOUND: \n");
        for (int i = 0; i < ArrayCapacity(engineState->soundChannels); i++)
        {
            SoundChannel* channel = &engineState->soundChannels[i];

            if (channel->playing)
            {
                StringAppend(text, "    ", (int)i, ", ");
                StringAppend(text, channel->sound->filename, ", ");
                StringAppend(text, ((float)channel->currentSample / (float)channel->sound->sampleCount), "\n");
            }
        }
        DrawText(engineState, text);
    }
}
