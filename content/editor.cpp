#pragma once

#include "haven.cpp"

namespace Editor
{
    const int tempStringSize = 2048;
    char text[tempStringSize] = {};
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

    float3 ScreenPointToRay(float2 screenPoint)
    {
        float2 mousePos = (screenPoint / haven->Resolution);
        mousePos = mousePos * 2.0 - 1.0;
        float3 pickerVector = haven->spectatorCamera.forward + 0.85 * mousePos.x * haven->spectatorCamera.right + 0.58 * mousePos.y * -haven->spectatorCamera.up;
        return normalize(pickerVector);
    }
    float2 PointToScreenPos(float3 position)
    {
        float3 localPos = WorldToLocal(position, haven->spectatorCamera);
        if (localPos.y < 0)
            return float2(-1, -1);
        float2 screenPos = (float2(localPos.x, -localPos.z) / localPos.y) * float2(0.58, 0.85) * haven->Resolution + haven->Resolution * 0.5;
        return screenPos;
    }

    float DistanceToCircle(float3 center, float3 normal, float radius, float2 testPos)
    {
        float3 direction = normalize(normal);
        float3 offangle = direction == float3(0, 0, 1) ? float3(0, 1, 0) : float3(0, 0, 1);
        float3 right = normalize(cross(direction, offangle));
        float3 left = normalize(cross(right, direction));
        float smallestDistance = 999999;
        int detail = 16;
        for (int i = 1; i < detail + 1; i++)
        {
            float angleStart = (i - 1) / (float)detail;
            float angleEnd = i / (float)detail;

            float3 offsetStart = AngleToVector(angleStart, left, right);
            float3 offsetEnd = AngleToVector(angleEnd, left, right);
            float3 start = center + offsetStart * radius;
            float3 end = center + offsetEnd * radius;
            float2 start2D = PointToScreenPos(start);
            float2 end2D = PointToScreenPos(end);
            float dist = DistanceToLine2D(start2D, end2D, input->mousePos);
            smallestDistance = min(smallestDistance, dist);
        }
        return smallestDistance;
    }
    float3 GetGizmoClickPoint(float2 mousePos, int ID, Transform axes)
    {
        // TODO: when pulling something far away, this breaks because the vector is pointing the wrong way
        float3 ray = ScreenPointToRay(mousePos);
        if (ID == 1) return ClosestRayApproach(haven->spectatorCamera.position, ray, axes.position, axes.right);
        if (ID == 2) return ClosestRayApproach(haven->spectatorCamera.position, ray, axes.position, axes.forward);
        if (ID == 3) return ClosestRayApproach(haven->spectatorCamera.position, ray, axes.position, axes.up);
        return float3(0, 0, 0);
    }
    float3 GetRotateGizmoClickPoint(float2 mousePos, int ID, Transform axes)
    {
        float3 ray = ScreenPointToRay(mousePos);
        if (ID == 1) return RayPlaneIntersectWorldSpace(haven->spectatorCamera.position, ray, axes.position, axes.right);
        if (ID == 2) return RayPlaneIntersectWorldSpace(haven->spectatorCamera.position, ray, axes.position, axes.forward);
        if (ID == 3) return RayPlaneIntersectWorldSpace(haven->spectatorCamera.position, ray, axes.position, axes.up);
        return float3(0, 0, 0);
    }

    bool local = false;
    bool gizmoCenteredOnBounds = true;

    void GizmoMove(Entity* entity)
    {
        Transform axes = transform(entity->transform.position, float3(1, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
        if (local)
            axes = entity->transform;
        if (gizmoCenteredOnBounds && entity->type == EntityType_StaticMesh)
            axes.position = StaticMeshGetLocalBoundsTransform((StaticMesh*)entity).position;

        int axis = 0;
        float dist0 = DistanceToLine2D(PointToScreenPos(axes.position), PointToScreenPos(axes.position + axes.right), input->mousePos);
        float dist1 = DistanceToLine2D(PointToScreenPos(axes.position), PointToScreenPos(axes.position + axes.forward), input->mousePos);
        float dist2 = DistanceToLine2D(PointToScreenPos(axes.position), PointToScreenPos(axes.position + axes.up), input->mousePos);
        if (min(dist0, dist1, dist2) == dist0 && dist0 < 25) axis = 1;
        if (min(dist0, dist1, dist2) == dist1 && dist1 < 25) axis = 2;
        if (min(dist0, dist1, dist2) == dist2 && dist2 < 25) axis = 3;

        float3 arrow0Color = float3(1, 0, 0);
        float3 arrow1Color = float3(0, 1, 0);
        float3 arrow2Color = float3(0, 0, 1);
        if(axis == 1) arrow0Color = float3(1, 1, 0);
        if(axis == 2) arrow1Color = float3(1, 1, 0);
        if (axis == 3) arrow2Color = float3(1, 1, 0);

        if (input->mouseLeftDown)
        {
            haven->gizmoAxis = axis;
        }
        if (input->mouseLeftUp)
        {
            haven->gizmoAxis = 0;
        }

        float3 lastClickPoint = GetGizmoClickPoint(input->mousePos - input->mousePosDelta, haven->gizmoAxis, axes);
        float3 clickPoint = GetGizmoClickPoint(input->mousePos, haven->gizmoAxis, axes);

        Debug::DrawArrow(axes.position, axes.position + axes.right     , 0.02, arrow0Color, 1, false, false);
        Debug::DrawArrow(axes.position, axes.position + axes.forward   , 0.02, arrow1Color, 1, false, false);
        Debug::DrawArrow(axes.position, axes.position + axes.up        , 0.02, arrow2Color, 1, false, false);

        if (input->mouseLeft)
        {
            entity->transform.position += clickPoint - lastClickPoint;
        }
    }

    void GizmoRotate(Entity* entity)
    {
        Transform axes = transform(entity->transform.position, float3(1, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
        if (local)
            axes = entity->transform;
        if (gizmoCenteredOnBounds && entity->type == EntityType_StaticMesh)
            axes.position = StaticMeshGetLocalBoundsTransform((StaticMesh*)entity).position;

        float dist0 = DistanceToCircle(axes.position, axes.right,     0.5, input->mousePos);
        float dist1 = DistanceToCircle(axes.position, axes.forward,   0.5, input->mousePos);
        float dist2 = DistanceToCircle(axes.position, axes.up,        0.5, input->mousePos);
        int axis = 0;
        if (min(dist0, dist1, dist2) == dist0 && dist0 < 50) axis = 1;
        if (min(dist0, dist1, dist2) == dist1 && dist1 < 50) axis = 2;
        if (min(dist0, dist1, dist2) == dist2 && dist2 < 50) axis = 3;

        if (input->mouseLeftDown)
        {
            haven->gizmoAxis = axis;
        }
        if (input->mouseLeftUp)
        {
            haven->gizmoAxis = 0;
        }

        float3 arrow0Color = float3(1, 0, 0);
        float3 arrow1Color = float3(0, 1, 0);
        float3 arrow2Color = float3(0, 0, 1);
        if (axis == 1) arrow0Color = float3(1, 1, 0);
        if (axis == 2) arrow1Color = float3(1, 1, 0);
        if (axis == 3) arrow2Color = float3(1, 1, 0);

        float3 lastClickPoint = GetRotateGizmoClickPoint(input->mousePos - input->mousePosDelta, haven->gizmoAxis, axes);
        float3 clickPoint = GetRotateGizmoClickPoint(input->mousePos, haven->gizmoAxis, axes);

        Debug::DrawCircle(axes.position, axes.right   , 0.5, 0.02, arrow0Color, 1, false);
        Debug::DrawCircle(axes.position, axes.forward , 0.5, 0.02, arrow1Color, 1, false);
        Debug::DrawCircle(axes.position, axes.up      , 0.5, 0.02, arrow2Color, 1, false);
        
        if (input->mouseLeft)
        {
            Debug::DrawLine(axes.position, axes.position+ normalize(clickPoint - axes.position) * 0.5, 0.05, float3(1, 1, 1), 1, false);
            entity->transform = rotateAboutPoint(entity->transform, axes.position, lastClickPoint - axes.position, clickPoint - axes.position);
        }
    }
    
    void GizmoScale(Entity* entity)
    {
        Transform axes = transform(entity->transform.position, float3(1, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
        if (local)
            axes = entity->transform;
        if (gizmoCenteredOnBounds && entity->type == EntityType_StaticMesh)
            axes.position = StaticMeshGetLocalBoundsTransform((StaticMesh*)entity).position;

        int axis = 0;
        float dist0 = DistanceToLine2D(PointToScreenPos(axes.position), PointToScreenPos(axes.position + axes.right), input->mousePos);
        float dist1 = DistanceToLine2D(PointToScreenPos(axes.position), PointToScreenPos(axes.position + axes.forward), input->mousePos);
        float dist2 = DistanceToLine2D(PointToScreenPos(axes.position), PointToScreenPos(axes.position + axes.up), input->mousePos);
        if (min(dist0, dist1, dist2) == dist0 && dist0 < 25) axis = 1;
        if (min(dist0, dist1, dist2) == dist1 && dist1 < 25) axis = 2;
        if (min(dist0, dist1, dist2) == dist2 && dist2 < 25) axis = 3;

        float3 arrow0Color = float3(1, 0, 0);
        float3 arrow1Color = float3(0, 1, 0);
        float3 arrow2Color = float3(0, 0, 1);
        if (axis == 1) arrow0Color = float3(1, 1, 0);
        if (axis == 2) arrow1Color = float3(1, 1, 0);
        if (axis == 3) arrow2Color = float3(1, 1, 0);

        if (input->mouseLeftDown)
        {
            haven->gizmoAxis = axis;
        }
        if (input->mouseLeftUp)
        {
            haven->gizmoAxis = 0;
        }

        float3 lastClickPoint = GetGizmoClickPoint(input->mousePos - input->mousePosDelta, haven->gizmoAxis, axes);
        float3 clickPoint = GetGizmoClickPoint(input->mousePos, haven->gizmoAxis, axes);

        Debug::DrawArrow(axes.position, axes.position + axes.right     , 0.02, arrow0Color, 1, false, true);
        Debug::DrawArrow(axes.position, axes.position + axes.forward   , 0.02, arrow1Color, 1, false, true);
        Debug::DrawArrow(axes.position, axes.position + axes.up        , 0.02, arrow2Color, 1, false, true);

        if (input->mouseLeft)
        {
            entity->transform.scale += clickPoint - lastClickPoint;
        }
    }
    void Update()
    {
        UpdateCamera();

        // mouse cursor
        Debug::DrawFont2D(".", input->mousePos, 500, 600, HAlign_left, VAlign_down);

        for (int i = 0; i < ArrayCount(haven->entities); i++)
        {
            haven->entities[i]->OverlayColor = float3(0, 0, 0);
        }

        float3 pickColor = float3(0.05f, 0.05f, 0.15f);
        //float3 selectedColor = float3(0.25f, 0.25f, 0.5f);
        float3 selectedColor = float3(0.0, 0.0, 0.0);

        if (haven->editor)
        {
            // Get the entity under the mouse cursor.
            haven->pickedEntity = 0;
            if (haven->gizmoAxis == 0)// not dragging anything
            {
                float2 mousePos = (input->mousePos / haven->Resolution);
                if (mousePos.x > 0 && mousePos.x < 1.0 && mousePos.y > 0 && mousePos.y < 1.0)
                {
                    float3 pickerVector = ScreenPointToRay(input->mousePos);

                    float3 PickerPosition = haven->spectatorCamera.position;

                    RayHit hit = StaticMeshLineTraceClosest(PickerPosition, pickerVector);
                    haven->pickedEntity = (Entity*)hit.entity;
                    Debug::DrawPoint(hit.position, 0.1f);
                }

                if (haven->pickedEntity)
                {
                    haven->pickedEntity->OverlayColor = pickColor;
                    if (input->mouseLeftUp)
                    {
                        haven->selectedEntity = haven->pickedEntity;
                    }
                }
            }
            if (haven->selectedEntity)
            {
                //Transform t = transform();
                //float3 a = RotateAroundAxis(float3(0, 0, 1), float3(0, 1, 0), 0.25);
                //haven->selectedEntity->transform = rotate(t, float3(0.5, 0, 0.5), a);
                int idx = -1;
                for (int i = 0; i < ArrayCount(haven->entities); i++)
                {
                    if (haven->selectedEntity == haven->entities[i])
                    {
                        idx = i;
                    }
                }

                EntityType type = (EntityType)haven->selectedEntity->type;

                haven->selectedEntity->OverlayColor = selectedColor;
                if (type == EntityType_StaticMesh)
                {
                    Debug::DrawBox(StaticMeshGetLocalBoundsTransform((StaticMesh*)haven->selectedEntity));
                }
                if (type == EntityType_ReflectionProbe)
                {
                    Debug::DrawBox(scale(haven->selectedEntity->transform, float3(0.25, 0.25, 0.25)));
                }
                Clear((uint8*)text, tempStringSize);
                StringAppend(text, "Selected Entity: \n");
                StringAppend(text, "Index: ");
                StringAppend(text, idx);
                StringAppend(text, "\nType: ");
                StringAppend(text, EntityTypeToString((EntityType)haven->selectedEntity->type));
                StringAppend(text, "\nPosition: ", haven->selectedEntity->transform.position);
                Debug::DrawText(text);
                
                if (haven->gizmoState == 0)
                {

                }
                else if (haven->gizmoState == 1)
                {
                    GizmoMove(haven->selectedEntity);
                }
                else if (haven->gizmoState == 2)
                {
                    GizmoRotate(haven->selectedEntity);
                }
                else if (haven->gizmoState == 3)
                {
                    GizmoScale(haven->selectedEntity);
                }
                
            }
            if (!input->mouseRight)
            {
                if (input->qDown)
                    haven->gizmoState = 0;
                if (input->wDown)
                    haven->gizmoState = 1;
                if (input->eDown)
                    haven->gizmoState = 2;
                if (input->rDown)
                    haven->gizmoState = 3;
            }

            Clear((uint8*)text, tempStringSize);
            StringAppend(text, "\n\nINPUT: ");
            StringAppend(text, "\n    Mouse pos: ", input->mousePos);
            StringAppend(text, "\n    Spectator pos: ", haven->spectatorCamera.position);
            Debug::DrawText(text);

            Clear((uint8*)text, tempStringSize);
            StringAppend(text, "    Left Controller:\n");
            StringAppend(text, "        Pos: ", input->handLeft.position);
            StringAppend(text, "\n        Trigger: ", input->triggerLeft);
            StringAppend(text, "\n        Thumbstick: ", input->thumbstickLeft);
            StringAppend(text, "\n        Face Button: ", input->faceButtonLeft);
            Debug::DrawText(text);

            Clear((uint8*)text, tempStringSize);
            StringAppend(text, "    Right Controller:\n");
            StringAppend(text, "        Pos: ", input->handRight.position);
            StringAppend(text, "\n        Trigger: ", input->triggerRight);
            StringAppend(text, "\n        Thumbstick: ", input->thumbstickRight);
            StringAppend(text, "\n        Face Button: ", input->faceButtonRight);
            Debug::DrawText(text);

            //haven->headsetView = DrawToggle(input, "Headset View: ", haven->headsetView);

            //if (DrawButton("Play a sound") || input->faceButtonLeftDown)
            //{
            //    PlaySound(assets->HitWoodDome_22k_mono, 1.0f, false);
            //}

            //Transform testTransform = transform(float3(12, 0, 0), 0.1, 0.2, 0.3, float3(1, 0.75, 0.5) * 0.75);
            /*
            Transform testTransform = scale(rotate(place(float3(12, 0, 0)), 0.1, 0.2, 0.3), float3(1, 0.75, 0.5) * 0.75);

            Debug::DrawLine(float3(4, 0, 0), float3(5, 1, 1), 0.05f, float3(0.6,0.1,0.2));
            Debug::DrawCircle(float3(6, 0, 0), float3(0, 0, 1), 0.75, 0.05f);
            Debug::DrawSphere(float3(8, 0, 0), 0.75, 0.05f);
            Debug::DrawAxisSphere(float3(10, 0, 0), 0.75, 0.05f);
            Debug::DrawPoint(float3(14, 0, 0), 0.25f);
            Debug::DrawAABB(float3(16, 0, 0), float3(1, 0.75, 0.5)* 0.75, 0.05f);
            Debug::DrawBox(testTransform, 0.05f);
            Debug::DrawBox2D(float2(200, 100), float2(100, 100), float3(0.5f, 0.0f, 0.0f), 0.5f);
            Debug::DrawBox2D(float2(250, 150), float2(100, 100), float3(0.0f, 0.5f, 0.0f), 0.5f);
            Debug::DrawBox2D(float2(300, 200), float2(100, 100), float3(0.0f, 0.0f, 0.5f), 0.5f);
        */
            Debug::DrawTransform(transform(float3(-1,-1,-1)), 0.05f);
        
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
            Debug::DrawText(text);
        }
    }
 }
