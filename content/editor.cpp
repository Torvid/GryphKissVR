#pragma once

#include "haven.cpp"

namespace Editor
{
    void Start()
    {

    }

    void UpdateCamera()
    {
        float mouseSensitivity = 0.1f;
        float scrollSensitivity = 0.5f;
        float panSensitivity = 0.01f;

        float fastMoveSpeed = 4.0f;
        float slowMoveSpeed = 16.0f;

        float mouseDeltaX = 0;
        float mouseDeltaY = 0;
        if (input->mouseRight)
        {
            mouseDeltaX = input->mousePosDelta.x;
            mouseDeltaY = input->mousePosDelta.y;
            if (haven->spectatorCamera.up.z < 0)
            {
                mouseDeltaX = -mouseDeltaX;
            }
        }
        haven->spectatorCamera = rotate(haven->spectatorCamera, float3(0, 0, 1), -mouseDeltaX * 0.005f * mouseSensitivity);
        haven->spectatorCamera = rotate(haven->spectatorCamera, haven->spectatorCamera.right, -mouseDeltaY * 0.005f * mouseSensitivity);

        float3 Movement = float3(0, 0, 0);
        if (input->mouseRight)
        {
            int x = input->w ? 1 : 0 + input->s ? -1 : 0;
            int y = input->d ? 1 : 0 + input->a ? -1 : 0;
            int z = input->e ? 1 : 0 + input->q ? -1 : 0;

            Movement += haven->spectatorCamera.forward * x;
            Movement += haven->spectatorCamera.right * y;
            Movement += haven->spectatorCamera.up * z;

            if (length(Movement) > 0)
                Movement = normalize(Movement);

            float speed = fastMoveSpeed;
            if (input->shift)
                speed = slowMoveSpeed;

            Movement *= input->deltaTime * speed;
        }

        if (input->mouseMiddle)
        {
            Movement -= haven->spectatorCamera.right * input->mousePosDelta.x * panSensitivity;
            Movement += haven->spectatorCamera.up * input->mousePosDelta.y * panSensitivity;
        }

        Movement += haven->spectatorCamera.forward * input->mouseScrollDelta * scrollSensitivity;
        
        haven->spectatorCamera.position += Movement;

    }


    void Update()
    {
        UpdateCamera();

        Drawing::DrawFont2D(".", input->mousePos, 500, 600, HAlign_left, VAlign_down);

        for (int i = 0; i < ArrayCount(haven->entities); i++)
        {
            haven->entities[i]->OverlayColor = float3(0, 0, 0);
        }

        if (haven->editor)
        {
            const int tempStringSize = 2048;
            char text[tempStringSize] = {};

            // Get the entity under the mouse cursor.
            haven->pickedEntity = 0;
            float2 mousePos = (input->mousePos / haven->Resolution);
            if (mousePos.x > 0 && mousePos.x < 1.0 && mousePos.y > 0 && mousePos.y < 1.0)
            {
                float2 a = mousePos * 2.0 - 1.0;

                Clear((uint8*)text, tempStringSize);
                StringAppend(text, "picker: ", a);
                Drawing::DrawText(text);

                float3 pickerVector = haven->spectatorCamera.forward + 0.85 * a.x * haven->spectatorCamera.right + 0.55 * a.y * -haven->spectatorCamera.up;

                float3 PickerPosition = haven->spectatorCamera.position;

                RayHit hit = StaticMeshLineTraceClosest(PickerPosition, pickerVector);
                haven->pickedEntity = (Entity*)hit.entity;
                Drawing::DrawPoint(hit.position, 0.1f);
            }

            float3 pickColor = float3(0.05f, 0.05f, 0.15f);
            float3 selectedColor = float3(0.25f, 0.25f, 0.5f) ;
            if (haven->pickedEntity)
            {
                haven->pickedEntity->OverlayColor = pickColor;
                if (input->mouseLeftDown)
                {
                    haven->selectedEntity = haven->pickedEntity;
                }
            }
            
            if (haven->selectedEntity)
            {
                haven->selectedEntity->OverlayColor = selectedColor;
                if (haven->selectedEntity->type == EntityType_StaticMesh)
                {
                    Drawing::DrawBox(StaticMeshGetLocalBoundsTransform((StaticMesh*)haven->selectedEntity));
                }
                if (haven->selectedEntity->type == EntityType_ReflectionProbe)
                {
                    Drawing::DrawBox(scale(haven->selectedEntity->transform, float3(0.25, 0.25, 0.25)));
                }
            }

            Clear((uint8*)text, tempStringSize);
            StringAppend(text, "\n\nINPUT: ");
            StringAppend(text, "\n    Mouse pos: ", input->mousePos);
            StringAppend(text, "\n    Spectator pos: ", haven->spectatorCamera.position);
            Drawing::DrawText(text);

            Clear((uint8*)text, tempStringSize);
            StringAppend(text, "    Left Controller:\n");
            StringAppend(text, "        Pos: ", input->handLeft.position);
            StringAppend(text, "\n        Trigger: ", input->triggerLeft);
            StringAppend(text, "\n        Thumbstick: ", input->thumbstickLeft);
            StringAppend(text, "\n        Face Button: ", input->faceButtonLeft);
            Drawing::DrawText(text);

            Clear((uint8*)text, tempStringSize);
            StringAppend(text, "    Right Controller:\n");
            StringAppend(text, "        Pos: ", input->handRight.position);
            StringAppend(text, "\n        Trigger: ", input->triggerRight);
            StringAppend(text, "\n        Thumbstick: ", input->thumbstickRight);
            StringAppend(text, "\n        Face Button: ", input->faceButtonRight);
            Drawing::DrawText(text);

            //haven->headsetView = DrawToggle(input, "Headset View: ", haven->headsetView);

            //if (DrawButton("Play a sound") || input->faceButtonLeftDown)
            //{
            //    PlaySound(assets->HitWoodDome_22k_mono, 1.0f, false);
            //}

            //Transform testTransform = transform(float3(12, 0, 0), 0.1, 0.2, 0.3, float3(1, 0.75, 0.5) * 0.75);
            /*
            Transform testTransform = scale(rotate(place(float3(12, 0, 0)), 0.1, 0.2, 0.3), float3(1, 0.75, 0.5) * 0.75);

            Drawing::DrawLine(float3(4, 0, 0), float3(5, 1, 1), 0.05f, float3(0.6,0.1,0.2));
            Drawing::DrawCircle(float3(6, 0, 0), float3(0, 0, 1), 0.75, 0.05f);
            Drawing::DrawSphere(float3(8, 0, 0), 0.75, 0.05f);
            Drawing::DrawAxisSphere(float3(10, 0, 0), 0.75, 0.05f);
            Drawing::DrawPoint(float3(14, 0, 0), 0.25f);
            Drawing::DrawAABB(float3(16, 0, 0), float3(1, 0.75, 0.5)* 0.75, 0.05f);
            Drawing::DrawBox(testTransform, 0.05f);
            Drawing::DrawBox2D(float2(200, 100), float2(100, 100), float3(0.5f, 0.0f, 0.0f), 0.5f);
            Drawing::DrawBox2D(float2(250, 150), float2(100, 100), float3(0.0f, 0.5f, 0.0f), 0.5f);
            Drawing::DrawBox2D(float2(300, 200), float2(100, 100), float3(0.0f, 0.0f, 0.5f), 0.5f);
        */
            Drawing::DrawTransform(transform(), 0.05f);
        
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
            Drawing::DrawText(text);
        }
    }
 }
