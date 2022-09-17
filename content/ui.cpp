

static const float fontScale = 2.0f;
static const float2 fontSpriteSize = { 6, 13 };


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
void DrawText(EngineState* engineState, char* text, float2* position, Transform transform, float3 color = { 1,1,1 })
{
    int len = StringLength(text);
    float2 cellCount = float2(32, 4);
    Vertex* vertexes = (Vertex*)engineState->scratchBuffer;
    uint32* indexes = (uint32*)engineState->scratchBuffer + len * 4 * sizeof(Vertex);

    int newlineCount = 1;
    int quadCount = 0;
    float3 offset = float3(0, 0, 0);
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
        vertexes[quadCount * 4 + 0] = { (float3(0, 0, 0) + offset) * float3(6, 13, 0), float3(0, 0, 0), (float2(0, 0) + pos) / cellCount };
        vertexes[quadCount * 4 + 1] = { (float3(1, 0, 0) + offset) * float3(6, 13, 0), float3(0, 0, 0), (float2(1, 0) + pos) / cellCount };
        vertexes[quadCount * 4 + 2] = { (float3(1, 1, 0) + offset) * float3(6, 13, 0), float3(0, 0, 0), (float2(1, 1) + pos) / cellCount };
        vertexes[quadCount * 4 + 3] = { (float3(0, 1, 0) + offset) * float3(6, 13, 0), float3(0, 0, 0), (float2(0, 1) + pos) / cellCount };

        indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
        indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
        indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
        indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
        indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
        indexes[quadCount * 6 + 5] = quadCount * 4 + 3;
        quadCount++;
        offset.x++;
    }
    Mesh* mesh = ArrayAddNewNoClear(engineState->stringMeshes);
    UpdateMesh(engineState, mesh, vertexes, quadCount * 4, indexes, quadCount * 6);

    Clear(engineState->scratchBuffer, Kilobytes(1));

    // Draw text mesh

    CreateMaterialLocal(command, engineState->UIShader, Material_UI);
    command->mesh = mesh;
    command->transform = transform;
    command->BackFaceCulling = false;
    command->Wireframe = false;
    command->DisableDepthTest = true;

    command->ColorTexture = engineState->fontTexture;
    command->Position = *position;
    command->Size = float2(1, 1) * fontScale;
    command->Color = color;



    DrawMesh(engineState, command, true);

    *position += float2(0, newlineCount * 13 * fontScale);
}

float2 GetTextSize(char* text)
{
    int len = StringLength(text);
    int height = StringCountOccurances(text, '\n');
    return float2(len, height + 1) * fontSpriteSize * fontScale;
}

bool DrawToggle(EngineState* engineState, Input* input, char* text, float2* position, Transform transform, bool value)
{
    char wat[100] = "[";
    StringAppend(wat, text);
    StringAppend(wat, value);
    StringAppend(wat, "]");
    float2 textSize = GetTextSize(wat);

    if (input->mousePos.x > position->x && input->mousePos.y > position->y &&
        input->mousePos.x < position->x + textSize.x && input->mousePos.y < position->y + textSize.y)
    {
        DrawText(engineState, wat, position, transform, float3(0.75, 0.75, 0.75));
        if (input->mouseLeftDown)
        {
            return !value;
        }
    }
    else
    {
        DrawText(engineState, wat, position, transform, float3(1, 1, 1));
    }

    return value;
}

bool DrawButton(EngineState* engineState, Input* input, char* text, float2* position, Transform transform)
{
    char wat[100] = "[";
    StringAppend(wat, text);
    StringAppend(wat, "]");
    float2 textSize = GetTextSize(wat);

    if (input->mousePos.x > position->x && input->mousePos.y > position->y &&
        input->mousePos.x < position->x + textSize.x && input->mousePos.y < position->y + textSize.y)
    {
        DrawText(engineState, wat, position, transform, float3(0.75, 0.75, 0.75));
        if (input->mouseLeftDown)
        {
            return true;
        }
    }
    else
    {
        DrawText(engineState, wat, position, transform, float3(1, 1, 1));
    }

    return false;
}

enum Anchor
{
    Anchor_TopLeft,
    Anchor_TopRight,
    Anchor_BottomLeft,
    Anchor_BottomRight,
    Anchor_Left,
    Anchor_Right,
    Anchor_Top,
    Anchor_Down,
};
void DrawBox2(float2 position, float2 size, float3 color, float alpha, Anchor anchor)
{

}

void DrawBox(EngineState* engineState, float2 position, float2 size, float3 color, Transform transform)
{
    CreateMaterialLocal(command, engineState->UIShader, Material_UI);
    command->mesh = engineState->ui_quad;
    command->Color = color;
    command->ColorTexture = engineState->texWhite;
    command->Opacity = 0.5f;
    

    command->blendMode = BlendMode_Alpha;
    command->Position = position;
    command->Size = size;
    command->transform = transform;
    command->BackFaceCulling = false;
    command->Wireframe = false;
    command->DisableDepthTest = true;

    DrawMesh(engineState, command);
}
