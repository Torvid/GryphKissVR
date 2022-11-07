#pragma once

#include "haven.cpp"

static const float fontScale = 1.0f;
static const float2 fontSpriteSize = { 6, 13 };
static const float2 atlasSize = { 32, 4 };
static const float2 glyphSize = { 6, 13 };

const int quadCountMax = 5000;
struct UIMeshData
{
    int quadCount;
    Vertex vertexes[quadCountMax * 3];
    uint32 indexes[quadCountMax * 6];
};


int GetFontIndex(char c)
{
    if (c >= 65 && c <= 90) // Capital letters
        return c - 65;

    if (c >= 97 && c <= 122) // Lowercase letters
        return (c - 97) + 32;

    if (c >= 48 && c <= 57) // Numbers
        return (c - 48) + 64;

    switch (c)
    {
        //case '£':  return 74;
    case '$':  return 75;
        //case '€':  return 76;
        //case '¥':  return 77;
    case '!':  return 96 + 0;
    case '?':  return 96 + 1;
    case '#':  return 96 + 2;
    case '&':  return 96 + 3;
    case '_':  return 96 + 4;
    case '%':  return 96 + 5;
    case '\'': return 96 + 6;
    case ',':  return 96 + 7;
    case '.':  return 96 + 8;
    case ';':  return 96 + 9;
    case ':':  return 96 + 10;
    case '^':  return 96 + 11;
    case '|':  return 96 + 12;
        //case '§':  return 96 + 13;
        //case '½':  return 96 + 14;
    case '`':  return 96 + 15;
        //case '´':  return 96 + 16;
    case '~':  return 96 + 17;
    case '=':  return 96 + 18;
    case '<':  return 96 + 19;
    case '>':  return 96 + 20;
    case '+':  return 96 + 21;
    case '-':  return 96 + 22;
    case '*':  return 96 + 23;
    case '/':  return 96 + 24;
    case '\\': return 96 + 25;
    case '(':  return 96 + 26;
    case ')':  return 96 + 27;
    case '{':  return 96 + 28;
    case '}':  return 96 + 29;
    case '[':  return 96 + 30;
    case ']':  return 96 + 31;
    case ' ':  return 78;
    case '\n': return -1;
    }
    return 78; // empty
}
float2 GetFontPos(int index)
{
    return float2(index % 32, index / 32);
}

void AppendQuadToUI3D(EngineState* engineState, float3 pivot, float3 position, float3 size, float2 UVpos, float2 UVsize, float3 color, float opacity)
{
    int quadCount = engineState->uiMeshData->quadCount;

    engineState->uiMeshData->vertexes[quadCount * 4 + 0] = { float3(0,  0,  0) * size + position, color, pivot, float2(0, 0) * UVsize + UVpos, float2(opacity, 1), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 1] = { float3(1,  0,  0) * size + position, color, pivot, float2(1, 0) * UVsize + UVpos, float2(opacity, 1), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 2] = { float3(1,  0,  1) * size + position, color, pivot, float2(1, 1) * UVsize + UVpos, float2(opacity, 1), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 3] = { float3(0,  0,  1) * size + position, color, pivot, float2(0, 1) * UVsize + UVpos, float2(opacity, 1), 0 };

    engineState->uiMeshData->indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
    engineState->uiMeshData->indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
    engineState->uiMeshData->indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
    engineState->uiMeshData->indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
    engineState->uiMeshData->indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
    engineState->uiMeshData->indexes[quadCount * 6 + 5] = quadCount * 4 + 3;

    engineState->uiMeshData->quadCount++;

    Assert(engineState->uiMeshData->quadCount < quadCountMax);
}

void AppendQuadToUI(EngineState* engineState, float2 position, float2 size, float2 UVpos, float2 UVsize, float3 color, float opacity)
{
    int quadCount = engineState->uiMeshData->quadCount;

    engineState->uiMeshData->vertexes[quadCount * 4 + 0] = { float3(0, 0, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, 0), color, float3(0, 0, 0), float2(0, 0) * UVsize + UVpos, float2(opacity, 0), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 1] = { float3(1, 0, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, 0), color, float3(0, 0, 0), float2(1, 0) * UVsize + UVpos, float2(opacity, 0), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 2] = { float3(1, 1, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, 0), color, float3(0, 0, 0), float2(1, 1) * UVsize + UVpos, float2(opacity, 0), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 3] = { float3(0, 1, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, 0), color, float3(0, 0, 0), float2(0, 1) * UVsize + UVpos, float2(opacity, 0), 0 };

    engineState->uiMeshData->indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
    engineState->uiMeshData->indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
    engineState->uiMeshData->indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
    engineState->uiMeshData->indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
    engineState->uiMeshData->indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
    engineState->uiMeshData->indexes[quadCount * 6 + 5] = quadCount * 4 + 3;

    engineState->uiMeshData->quadCount++;

    Assert(engineState->uiMeshData->quadCount < quadCountMax);
}

void AppendSquareShapetoUI3D(EngineState* engineState, float3 pos0, float3 pos1, float3 pos2, float3 pos3, float3 color, float opacity)
{
    int quadCount = engineState->uiMeshData->quadCount;

    engineState->uiMeshData->vertexes[quadCount * 4 + 0] = { pos0, color, float3(1, 0, 0), float2(0, 0) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 2), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 1] = { pos1, color, float3(1, 0, 0), float2(1, 0) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 2), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 2] = { pos2, color, float3(1, 0, 0), float2(1, 1) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 2), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 3] = { pos3, color, float3(1, 0, 0), float2(0, 1) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 2), 0 };

    engineState->uiMeshData->indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
    engineState->uiMeshData->indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
    engineState->uiMeshData->indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
    engineState->uiMeshData->indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
    engineState->uiMeshData->indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
    engineState->uiMeshData->indexes[quadCount * 6 + 5] = quadCount * 4 + 3;

    engineState->uiMeshData->quadCount++;

    Assert(engineState->uiMeshData->quadCount < quadCountMax);
}

void AppendSquareShapetoUI(EngineState* engineState, float2 pos0, float2 pos1, float2 pos2, float2 pos3, float3 color, float opacity)
{
    int quadCount = engineState->uiMeshData->quadCount;

    engineState->uiMeshData->vertexes[quadCount * 4 + 0] = { float3(pos0.x, pos0.y, 0), color, float3(1, 0, 0), float2(0, 0) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 0), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 1] = { float3(pos1.x, pos1.y, 0), color, float3(1, 0, 0), float2(1, 0) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 0), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 2] = { float3(pos2.x, pos2.y, 0), color, float3(1, 0, 0), float2(1, 1) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 0), 0 };
    engineState->uiMeshData->vertexes[quadCount * 4 + 3] = { float3(pos3.x, pos3.y, 0), color, float3(1, 0, 0), float2(0, 1) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 0), 0 };

    engineState->uiMeshData->indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
    engineState->uiMeshData->indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
    engineState->uiMeshData->indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
    engineState->uiMeshData->indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
    engineState->uiMeshData->indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
    engineState->uiMeshData->indexes[quadCount * 6 + 5] = quadCount * 4 + 3;

    engineState->uiMeshData->quadCount++;

    Assert(engineState->uiMeshData->quadCount < quadCountMax);
}


void DrawText3D(EngineState* engineState, char* text, float3 position, float scale = 1.0f, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    int newlineCount = 1;
    float2 offset = float2(0, 0);
    float totalLength = StringLength(text) * 0.5f;
    for (int i = 0; i < StringLength(text); i++)
    {
        int index = GetFontIndex(text[i]);
        float2 pos = GetFontPos(index);

        if (index == -1) // newline
        {
            offset.x = 0;
            offset.y += 1;
            continue;
        }
        float3 size = float3(glyphSize.x, glyphSize.y, glyphSize.y) * 0.01f * scale;
        float3 offsetSize = float3(offset.x- totalLength, offset.y - 0.5f, offset.y - 0.5f);
        AppendQuadToUI3D(engineState, position, (offsetSize * size), size, (pos / atlasSize), float2(1, 1) / atlasSize, color, opacity);

        offset.x++;
    }
}

void DrawText(EngineState* engineState, char* text, float2* position, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    int newlineCount = 1;
    float2 offset = float2(0, 0);
    for (int i = 0; i < StringLength(text); i++)
    {
        int index = GetFontIndex(text[i]);
        float2 pos = GetFontPos(index);

        if (index == -1) // newline
        {
            offset.x = 0;
            offset.y += 1;
            newlineCount++;
            continue;
        }
        
        AppendQuadToUI(engineState, offset * glyphSize + floor(*position), glyphSize, pos / atlasSize, float2(1, 1) / atlasSize, color, opacity);

        offset.x++;
    }

    *position += float2(0, newlineCount * 13 * fontScale);
}
void DrawText(EngineState* engineState, char* text, float2 position, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawText(engineState, text, &position, color, opacity);
}
void DrawText(EngineState* engineState, char* text, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawText(engineState, text, &engineState->uiPos, color, opacity);
}

float2 GetTextSize(char* text)
{
    int len = StringLength(text);
    int height = StringCountOccurances(text, '\n');
    return float2(len, height + 1) * fontSpriteSize * fontScale;
}


bool DrawToggle(EngineState* engineState, Input* input, char* text, float2* position, bool value)
{
    char wat[100] = "[";
    StringAppend(wat, text);
    StringAppend(wat, value);
    StringAppend(wat, "]");
    float2 textSize = GetTextSize(wat);

    if (input->mousePos.x > position->x && input->mousePos.y > position->y &&
        input->mousePos.x < position->x + textSize.x && input->mousePos.y < position->y + textSize.y)
    {
        DrawText(engineState, wat, position, float3(0.75, 0.75, 0.75));
        if (input->mouseLeftDown)
        {
            return !value;
        }
    }
    else
    {
        DrawText(engineState, wat, position, float3(1, 1, 1));
    }

    return value;
}
bool DrawToggle(EngineState* engineState, Input* input, char* text, bool value)
{
    return DrawToggle(engineState, input, text, &engineState->uiPos, value);
}

bool DrawButton(EngineState* engineState, Input* input, char* text, float2* position)
{
    char wat[100] = "[";
    StringAppend(wat, text);
    StringAppend(wat, "]");
    float2 textSize = GetTextSize(wat);

    if (input->mousePos.x > position->x && input->mousePos.y > position->y &&
        input->mousePos.x < position->x + textSize.x && input->mousePos.y < position->y + textSize.y)
    {
        DrawText(engineState, wat, position, float3(0.75, 0.75, 0.75));
        if (input->mouseLeftDown)
        {
            return true;
        }
    }
    else
    {
        DrawText(engineState, wat, position, float3(1, 1, 1));
    }

    return false;
}
bool DrawButton(EngineState* engineState, Input* input, char* text)
{
    return DrawButton(engineState, input, text, &engineState->uiPos);
}

void DrawBox2D(EngineState* engineState, float2 position, float2 size, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    AppendQuadToUI(engineState, position, size, float2(0.95, 0.01), float2(0.01, 0.1), color, opacity);
}
void DrawLine2D(EngineState* engineState, float2 start, float2 end, float width, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    float2 side = normalize(rotate90CW(start - end)) * width * 0.5f;
    
    float2 pos0 = start - side;
    float2 pos1 = start + side;
    float2 pos2 = end + side;
    float2 pos3 = end - side;

    AppendSquareShapetoUI(engineState, pos0, pos1, pos2, pos3, color, opacity);
}


void DrawLine(EngineState* engineState, float3 start, float3 end, float width = 0.01f, float3 color = {1, 1, 1}, float opacity = 1)
{
    float3 direction = (end - start);
    float3 q = cross(direction, float3(0, 0, 1));
    float3 offangle = ((q.x + q.y + q.z) == 0) ? float3(0, 1, 0) : float3(0, 0, 1);
    float3 right = normalize(cross(direction, offangle)) * width * 0.5;
    float3 left = normalize(cross(right, direction)) * width * 0.5;

    float3 pos0 = end + left;
    float3 pos1 = start + left; 
    float3 pos2 = start - left;
    float3 pos3 = end - left; 

    AppendSquareShapetoUI3D(engineState, pos0, pos1, pos2, pos3, color, opacity);

    pos0 = end + right;
    pos1 = start + right;
    pos2 = start - right;
    pos3 = end - right;
    AppendSquareShapetoUI3D(engineState, pos0, pos1, pos2, pos3, color, opacity);

}

void DrawRay(EngineState* engineState, float3 start, float3 direction, float len, float width = 0.01f, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawLine(engineState, start, start + normalize(direction) * len, width, color, opacity);
}

float3 AngleToVector(float angle, float3 left, float3 right)
{
    return sin_turns(angle) * left + cos_turns(angle) * right;
}

void DrawCircle(EngineState* engineState, float3 center, float3 normal, float radius, float width, float3 color = { 1, 1, 1 }, float opacity = 1, int detail = 32)
{
    float3 direction = normalize(normal);
    float3 offangle = direction == float3(0, 0, 1) ? float3(0, 1, 0) : float3(0, 0, 1);
    float3 right = normalize(cross(direction, offangle));
    float3 left = normalize(cross(right, direction));
    width *= 0.5;
    //int detail = 32;
    for (int i = 1; i < detail+1; i++)
    {
        float angleStart = (i-1) / (float)detail;
        float angleEnd = i / (float)detail;

        float3 offsetStart = AngleToVector(angleStart, left, right);
        float3 offsetEnd   = AngleToVector(angleEnd, left, right);

        float3 pos0 = center + offsetStart * radius - offsetEnd   * width;
        float3 pos1 = center + offsetEnd   * radius - offsetStart * width;
        float3 pos2 = center + offsetEnd   * radius + offsetEnd   * width;
        float3 pos3 = center + offsetStart * radius + offsetStart * width;
        AppendSquareShapetoUI3D(engineState, pos0, pos1, pos2, pos3, color, opacity);

        pos0 = center + offsetStart * radius - direction * width;
        pos1 = center + offsetEnd   * radius - direction * width;
        pos2 = center + offsetEnd   * radius + direction * width;
        pos3 = center + offsetStart * radius + direction * width;
        AppendSquareShapetoUI3D(engineState, pos0, pos1, pos2, pos3, color, opacity);
    }
}

void DrawSphere(EngineState* engineState, float3 center, float radius, float width, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    for (int i = 0; i < 8; i++)
    {
        float3 a = AngleToVector(i / 8.0f, float3(1, 0, 0), float3(0, 1, 0));
        DrawCircle(engineState, center, a, radius, width, color, opacity, 8);
    }
    DrawCircle(engineState, center + float3(0, 0, radius * 0.707), float3(0, 0, 1), radius* 0.707, width, color, opacity, 8);
    DrawCircle(engineState, center, float3(0, 0, 1), radius, width, color, opacity, 8);
    DrawCircle(engineState, center + float3(0, 0, -radius * 0.707), float3(0, 0, 1), radius * 0.707, width, color, opacity, 8);
}

void DrawAxisSphere(EngineState* engineState, float3 center, float radius, float width, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawCircle(engineState, center, float3(1, 0, 0), radius, width, color, opacity, 8);
    DrawCircle(engineState, center, float3(0, 1, 0), radius, width, color, opacity, 8);
    DrawCircle(engineState, center, float3(0, 0, 1), radius, width, color, opacity, 8);
}

void DrawTransform(EngineState* engineState, Transform transform, float width)
{
    DrawRay(engineState, transform.position, transform.right,   transform.scale.x, width, float3(1, 0, 0));
    DrawRay(engineState, transform.position, transform.forward, transform.scale.y, width, float3(0, 1, 0));
    DrawRay(engineState, transform.position, transform.up,      transform.scale.z, width, float3(0, 0, 1));
}

void DrawPoint(EngineState* engineState, float3 position, float width, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawLine(engineState, position - float3(width * 0.5, 0, 0), position + float3(width * 0.5, 0, 0), width, color, opacity);
    DrawLine(engineState, position - float3(0, width * 0.5, 0), position + float3(0, width * 0.5, 0), width, color, opacity);
}

void DrawAABB(EngineState* engineState, float3 center, float3 size, float width, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawLine(engineState, center + size * float3( 1,  1,  1), center + size * float3( 1,  1, -1), width, color, opacity);
    DrawLine(engineState, center + size * float3(-1,  1,  1), center + size * float3(-1,  1, -1), width, color, opacity);
    DrawLine(engineState, center + size * float3( 1, -1,  1), center + size * float3( 1, -1, -1), width, color, opacity);
    DrawLine(engineState, center + size * float3(-1, -1,  1), center + size * float3(-1, -1, -1), width, color, opacity);

    DrawLine(engineState, center + size * float3( 1,  1,  1), center + size * float3( 1, -1,  1), width, color, opacity);
    DrawLine(engineState, center + size * float3(-1,  1,  1), center + size * float3(-1, -1,  1), width, color, opacity);
    DrawLine(engineState, center + size * float3( 1,  1, -1), center + size * float3( 1, -1, -1), width, color, opacity);
    DrawLine(engineState, center + size * float3(-1,  1, -1), center + size * float3(-1, -1, -1), width, color, opacity);

    DrawLine(engineState, center + size * float3(1,  1,   1), center + size * float3(-1,  1,  1), width, color, opacity);
    DrawLine(engineState, center + size * float3(1, -1,   1), center + size * float3(-1, -1,  1), width, color, opacity);
    DrawLine(engineState, center + size * float3(1,  1,  -1), center + size * float3(-1,  1, -1), width, color, opacity);
    DrawLine(engineState, center + size * float3(1, -1,  -1), center + size * float3(-1, -1, -1), width, color, opacity);
}

float3 TransformPosition_LocalSpaceToWorldSpace(Transform transform, float3 localPosition)
{
    // Multiply the x/y/z components with the transforms axes.
    // Since local-position is local, its component values represent movements along the axes of the transform.
    // So moving along those axes bring us into world-space.
    float3 worldPos = transform.right   * localPosition.x * transform.scale.x +
                      transform.forward * localPosition.y * transform.scale.y +
                      transform.up      * localPosition.z * transform.scale.z;

    // Move the point from being relative to the transform to being placed in the world
    return worldPos + transform.position;
}

float3 TransformPosition_WorldSpaceToLocalSpace(Transform transform, float3 worldPosition)
{
    // Move the point to be relative to the transform.
    float3 localPos = worldPosition - transform.position;

    // Project the moved position onto each of the transforms axes.
    // This "rotates" it from world space to local space.
    return float3(dot(localPos, transform.right), 
                  dot(localPos, transform.forward), 
                  dot(localPos, transform.up));
}

void DrawBox(EngineState* engineState, Transform transform,  float width, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1, -1)), width, color, opacity);
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1, -1)), width, color, opacity);
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1, -1)), width, color, opacity);
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1, -1)), width, color, opacity);
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1,  1)), width, color, opacity);
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1,  1)), width, color, opacity);
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1, -1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1, -1)), width, color, opacity);
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1, -1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1, -1)), width, color, opacity);
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1,  1)), width, color, opacity);
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1,  1)), width, color, opacity);
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1, -1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1, -1)), width, color, opacity);
    DrawLine(engineState, TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1, -1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1, -1)), width, color, opacity);
}
