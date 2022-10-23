#pragma once

#include "haven.cpp"

static const float fontScale = 1.0f;
static const float2 fontSpriteSize = { 6, 13 };

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

void AppendQuadToUI(EngineState* engineState, float2 position, float2 size, float2 UVpos, float2 UVsize, float3 color, float opacity)
{
    int quadCount = engineState->uiMeshData->quadCount;

    engineState->uiMeshData->vertexes[quadCount * 4 + 0] = { float3(0, 0, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, opacity), color, float3(1, 0, 0), float2(0, 0) * UVsize + UVpos };
    engineState->uiMeshData->vertexes[quadCount * 4 + 1] = { float3(1, 0, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, opacity), color, float3(1, 0, 0), float2(1, 0) * UVsize + UVpos };
    engineState->uiMeshData->vertexes[quadCount * 4 + 2] = { float3(1, 1, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, opacity), color, float3(1, 0, 0), float2(1, 1) * UVsize + UVpos };
    engineState->uiMeshData->vertexes[quadCount * 4 + 3] = { float3(0, 1, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, opacity), color, float3(1, 0, 0), float2(0, 1) * UVsize + UVpos };

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

    engineState->uiMeshData->vertexes[quadCount * 4 + 0] = { float3(pos0.x, pos0.y, opacity), color, float3(1, 0, 0), float2(0, 0) * float2(0.01, 0.1) + float2(0.95, 0.01) };
    engineState->uiMeshData->vertexes[quadCount * 4 + 1] = { float3(pos1.x, pos1.y, opacity), color, float3(1, 0, 0), float2(1, 0) * float2(0.01, 0.1) + float2(0.95, 0.01) };
    engineState->uiMeshData->vertexes[quadCount * 4 + 2] = { float3(pos2.x, pos2.y, opacity), color, float3(1, 0, 0), float2(1, 1) * float2(0.01, 0.1) + float2(0.95, 0.01) };
    engineState->uiMeshData->vertexes[quadCount * 4 + 3] = { float3(pos3.x, pos3.y, opacity), color, float3(1, 0, 0), float2(0, 1) * float2(0.01, 0.1) + float2(0.95, 0.01) };

    engineState->uiMeshData->indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
    engineState->uiMeshData->indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
    engineState->uiMeshData->indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
    engineState->uiMeshData->indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
    engineState->uiMeshData->indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
    engineState->uiMeshData->indexes[quadCount * 6 + 5] = quadCount * 4 + 3;

    engineState->uiMeshData->quadCount++;

    Assert(engineState->uiMeshData->quadCount < quadCountMax);
}

void DrawText(EngineState* engineState, char* text, float2* position, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    int len = StringLength(text);
    float2 atlasSize = float2(32, 4);
    float2 glyphSize = float2(6, 13);

    int newlineCount = 1;
    float2 offset = float2(0, 0);
    for (int i = 0; i < len; i++)
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

void DrawBox(EngineState* engineState, float2 position, float2 size, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    AppendQuadToUI(engineState, position, size, float2(0.95, 0.01), float2(0.01, 0.1), color, opacity);
}
void DrawLine(EngineState* engineState, float2 start, float2 end, float width, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    float2 side = normalize(rotate90CW(start - end)) * width * 0.5f;
    
    float2 pos0 = start - side;
    float2 pos1 = start + side;
    float2 pos2 = end + side;
    float2 pos3 = end - side;

    AppendSquareShapetoUI(engineState, pos0, pos1, pos2, pos3, color, opacity);
}
