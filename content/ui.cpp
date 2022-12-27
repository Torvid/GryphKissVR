#pragma once

#include "haven.cpp"

static const float fontScale = 1.0f;
static const float2 fontSpriteSize = { 6, 13 };
static const float2 atlasSize = { 32, 4 };
static const float2 glyphSize = { 6, 13 };

const int quadCountMax = 50000;
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

void AppendQuadToUI3D(float3 pivot, float3 position, float3 size, float2 UVpos, float2 UVsize, float3 color, float opacity, int texture = 0)
{
    int quadCount = haven->uiMeshData->quadCount;

    haven->uiMeshData->vertexes[quadCount * 4 + 0] = { float3(0,  0,  0) * size + position, color, pivot, float2(0, 0) * UVsize + UVpos, float2(opacity, 2), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 1] = { float3(1,  0,  0) * size + position, color, pivot, float2(1, 0) * UVsize + UVpos, float2(opacity, 2), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 2] = { float3(1,  0,  1) * size + position, color, pivot, float2(1, 1) * UVsize + UVpos, float2(opacity, 2), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 3] = { float3(0,  0,  1) * size + position, color, pivot, float2(0, 1) * UVsize + UVpos, float2(opacity, 2), texture };

    haven->uiMeshData->indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
    haven->uiMeshData->indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 5] = quadCount * 4 + 3;

    haven->uiMeshData->quadCount++;

    Assert(haven->uiMeshData->quadCount < quadCountMax);
}

void AppendQuadToUI3DCameraFacing(float3 pivot, float3 position, float3 size, float2 UVpos, float2 UVsize, float3 color, float opacity, int texture = 0)
{
   int quadCount = haven->uiMeshData->quadCount;

    haven->uiMeshData->vertexes[quadCount * 4 + 0] = { float3(0,  0,  0) * size + position, color, pivot, float2(0, 0) * UVsize + UVpos, float2(opacity, 1), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 1] = { float3(1,  0,  0) * size + position, color, pivot, float2(1, 0) * UVsize + UVpos, float2(opacity, 1), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 2] = { float3(1,  0,  1) * size + position, color, pivot, float2(1, 1) * UVsize + UVpos, float2(opacity, 1), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 3] = { float3(0,  0,  1) * size + position, color, pivot, float2(0, 1) * UVsize + UVpos, float2(opacity, 1), texture };

    haven->uiMeshData->indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
    haven->uiMeshData->indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 5] = quadCount * 4 + 3;

    haven->uiMeshData->quadCount++;

    Assert(haven->uiMeshData->quadCount < quadCountMax);
}

void AppendQuadToUI(float2 position, float2 size, float2 UVpos, float2 UVsize, float3 color, float opacity, int texture = 0)
{
    int quadCount = haven->uiMeshData->quadCount;

    haven->uiMeshData->vertexes[quadCount * 4 + 0] = { float3(0, 0, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, 0), color, float3(0, 0, 0), float2(0, 0) * UVsize + UVpos, float2(opacity, 0), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 1] = { float3(1, 0, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, 0), color, float3(0, 0, 0), float2(1, 0) * UVsize + UVpos, float2(opacity, 0), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 2] = { float3(1, 1, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, 0), color, float3(0, 0, 0), float2(1, 1) * UVsize + UVpos, float2(opacity, 0), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 3] = { float3(0, 1, 0) * float3(size.x, size.y, 0) + float3(position.x, position.y, 0), color, float3(0, 0, 0), float2(0, 1) * UVsize + UVpos, float2(opacity, 0), texture };

    haven->uiMeshData->indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
    haven->uiMeshData->indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 5] = quadCount * 4 + 3;

    haven->uiMeshData->quadCount++;

    Assert(haven->uiMeshData->quadCount < quadCountMax);
}

void AppendSquareShapetoUI3D(float3 pos0, float3 pos1, float3 pos2, float3 pos3, float3 color, float opacity, int texture = 0)
{
    int quadCount = haven->uiMeshData->quadCount;

    haven->uiMeshData->vertexes[quadCount * 4 + 0] = { pos0, color, float3(1, 0, 0), float2(0, 0) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 2), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 1] = { pos1, color, float3(1, 0, 0), float2(1, 0) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 2), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 2] = { pos2, color, float3(1, 0, 0), float2(1, 1) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 2), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 3] = { pos3, color, float3(1, 0, 0), float2(0, 1) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 2), texture };

    haven->uiMeshData->indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
    haven->uiMeshData->indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 5] = quadCount * 4 + 3;

    haven->uiMeshData->quadCount++;

    Assert(haven->uiMeshData->quadCount < quadCountMax);
}

void AppendSquareShapetoUI3D(float3 pos0, float3 pos1, float3 pos2, float3 pos3, float2 uv0, float2 uv1, float2 uv2, float2 uv3, float3 color, float opacity, int texture = 0)
{
    int quadCount = haven->uiMeshData->quadCount;

    haven->uiMeshData->vertexes[quadCount * 4 + 0] = { pos0, color, float3(1, 0, 0), uv0, float2(opacity, 2), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 1] = { pos1, color, float3(1, 0, 0), uv1, float2(opacity, 2), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 2] = { pos2, color, float3(1, 0, 0), uv2, float2(opacity, 2), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 3] = { pos3, color, float3(1, 0, 0), uv3, float2(opacity, 2), texture };

    haven->uiMeshData->indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
    haven->uiMeshData->indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 5] = quadCount * 4 + 3;

    haven->uiMeshData->quadCount++;

    Assert(haven->uiMeshData->quadCount < quadCountMax);
}
void AppendSquareShapetoUI(float2 pos0, float2 pos1, float2 pos2, float2 pos3, float3 color, float opacity, int texture = 0)
{
    int quadCount = haven->uiMeshData->quadCount;

    haven->uiMeshData->vertexes[quadCount * 4 + 0] = { float3(pos0.x, pos0.y, 0), color, float3(1, 0, 0), float2(0, 0) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 0), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 1] = { float3(pos1.x, pos1.y, 0), color, float3(1, 0, 0), float2(1, 0) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 0), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 2] = { float3(pos2.x, pos2.y, 0), color, float3(1, 0, 0), float2(1, 1) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 0), texture };
    haven->uiMeshData->vertexes[quadCount * 4 + 3] = { float3(pos3.x, pos3.y, 0), color, float3(1, 0, 0), float2(0, 1) * float2(0.01, 0.1) + float2(0.95, 0.01), float2(opacity, 0), texture };

    haven->uiMeshData->indexes[quadCount * 6 + 0] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 1] = quadCount * 4 + 1;
    haven->uiMeshData->indexes[quadCount * 6 + 2] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 3] = quadCount * 4 + 0;
    haven->uiMeshData->indexes[quadCount * 6 + 4] = quadCount * 4 + 2;
    haven->uiMeshData->indexes[quadCount * 6 + 5] = quadCount * 4 + 3;

    haven->uiMeshData->quadCount++;

    Assert(haven->uiMeshData->quadCount < quadCountMax);
}


void DrawText3D(char* text, float3 position, float scale = 1.0f, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    int newlineCount = 1;
    float2 offset = float2(0, 0);
    float totalLength = 0;// StringLength(text);
    for (int i = 0; i < StringLength(text); i++)
    {
        int dist = DistanceToNewline(&text[i]);
        i += dist;
        totalLength = max(totalLength, dist);
    }
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
        float3 size = float3(glyphSize.x, 0, glyphSize.y) * 0.01f * scale;
        float3 offsetSize = float3(offset.x - totalLength * 0.5f, 0, offset.y - 0.5f);
        AppendQuadToUI3DCameraFacing(position, (offsetSize * size), size, (pos / atlasSize), float2(1, 1) / atlasSize, color, opacity);
        //AppendQuadToUI3D(position, position + (offsetSize * size), size, (pos / atlasSize), float2(1, 1) / atlasSize, color, opacity);

        offset.x++;
    }
}

void DrawText(char* text, float2* position, float3 color = { 1, 1, 1 }, float opacity = 1)
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
        float2 fontResolutionInv = 1.0f / float2(192, 52);
        AppendQuadToUI(offset * glyphSize + floor(*position), glyphSize - float2(0, 1), pos / (atlasSize), float2(1, 1) / (atlasSize)-float2(0, fontResolutionInv.y), color, opacity);

        offset.x++;
    }

    *position += float2(0, newlineCount * 13 * fontScale);
}
void DrawText(char* text, float2 position, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawText(text, &position, color, opacity);
}
void DrawText(char* text, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawText(text, &haven->uiPos, color, opacity);
}
void DrawBox2D(float2 position, float2 size, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    AppendQuadToUI(position, size, float2(0.95, 0.01), float2(0.01, 0.1), color, opacity);
}
void DrawLine2D(float2 start, float2 end, float width = 0.01, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    float2 side = normalize(rotate90CW(start - end)) * width * 0.5f;

    float2 pos0 = start - side;
    float2 pos1 = start + side;
    float2 pos2 = end + side;
    float2 pos3 = end - side;

    AppendSquareShapetoUI(pos0, pos1, pos2, pos3, color, opacity);
}



Glyph* GetGlyph(Font* font, char c)
{
    if (c >= 65 && c <= 90) // Capital letters
        return &(font->glyphs[c - 65 + 36]);

    if (c >= 97 && c <= 122) // Lowercase letters
        return &(font->glyphs[c - 97 + 10]);

    if (c >= 48 && c <= 57) // Numbers
        return &(font->glyphs[c - 48]);

    int i = 10 + 27 + 27-2;
    switch (c)
    {
        case '$':  return  &(font->glyphs[i + 0]);
        case '!':  return  &(font->glyphs[i + 1]);
        case '?':  return  &(font->glyphs[i + 2]);
        case '#':  return  &(font->glyphs[i + 3]);
        case '&':  return  &(font->glyphs[i + 4]);
        case '_':  return  &(font->glyphs[i + 5]);
        case '%':  return  &(font->glyphs[i + 6]);
        case '\'':  return &(font->glyphs[i + 7]);
        case '\"':  return &(font->glyphs[i + 8]);
        case ',':  return  &(font->glyphs[i + 9]);
        case '.':  return  &(font->glyphs[i + 10]);
        case ';':  return  &(font->glyphs[i + 11]);
        case ':':  return  &(font->glyphs[i + 12]);
        case '^':  return  &(font->glyphs[i + 13]);
        case '|':  return  &(font->glyphs[i + 14]);
        case '`':  return  &(font->glyphs[i + 15]);
        case '~':  return  &(font->glyphs[i + 16]);
        case '=':  return  &(font->glyphs[i + 17]);
        case '<':  return  &(font->glyphs[i + 18]);
        case '>':  return  &(font->glyphs[i + 19]);
        case '+':  return  &(font->glyphs[i + 20]);
        case '-':  return  &(font->glyphs[i + 21]);
        case '*':  return  &(font->glyphs[i + 22]);
        case '/':  return  &(font->glyphs[i + 23]);
        case '\\':  return &(font->glyphs[i + 24]);
        case '(':  return  &(font->glyphs[i + 25]);
        case ')':  return  &(font->glyphs[i + 26]);
        case '{':  return  &(font->glyphs[i + 27]);
        case '}':  return  &(font->glyphs[i + 28]);
        case '[':  return  &(font->glyphs[i + 29]);
        case ']':  return  &(font->glyphs[i + 30]);
        case '@':  return  &(font->glyphs[i + 31]);
    }

    return &(font->glyphs[i + 2]);
}

bool IsHexNumber(char c)
{
    if (c >= 48 && c <= 57)
        return true;
    if (c >= 97 && c <= 103)
        return true;
    return false;
}
int CharToNumber(char c)
{
    if (c >= 48 && c <= 57)
        return c - 48;
    if (c >= 97 && c <= 103)
        return c - 97 + 10;
    return 0;
}

int EscapeSequenceBold(char* text)
{
    if (text[0] == '*' && text[1] == '*')
        return 2;
    return 0;
}

int EscapeSequenceItalic(char* text)
{
    if (text[0] == '*' && text[1] != '*' && text[-1] != '*')
        return 1;
    return 0;
}

int EscapeSequenceColor(char* text)
{
    if (text[0] == '#' && text[3] == '#')
    {
        if (!IsHexNumber(text[1])) return 0;
        if (!IsHexNumber(text[2])) return 0;
        return 4;
    }
    if (text[0] == '#' && text[7] == '#')
    {
        if (!IsHexNumber(text[1])) return 0;
        if (!IsHexNumber(text[2])) return 0;
        if (!IsHexNumber(text[3])) return 0;
        if (!IsHexNumber(text[4])) return 0;
        if (!IsHexNumber(text[5])) return 0;
        if (!IsHexNumber(text[6])) return 0;
        return 8;
    }
    if (text[0] == '#' && text[9] == '#')
    {
        if (!IsHexNumber(text[1])) return 0;
        if (!IsHexNumber(text[2])) return 0;
        if (!IsHexNumber(text[3])) return 0;
        if (!IsHexNumber(text[4])) return 0;
        if (!IsHexNumber(text[5])) return 0;
        if (!IsHexNumber(text[6])) return 0;
        if (!IsHexNumber(text[7])) return 0;
        if (!IsHexNumber(text[8])) return 0;
        return 10;
    }
    return 0;
}


float newlineHeight = 0.15;
float spaceWidth = 0.05;
bool IncrementGlyph(char c, Font* font, float2* pos, float2 startPos, float scale)
{
    if (c == ' ')
    {
        *pos += float2(spaceWidth * scale, 0.0f);
        return true;
    }
    if (c == '\n')
    {
        (*pos).y += newlineHeight * scale;
        (*pos).x = startPos.x;
        return false;
    }
    Glyph* glyph = GetGlyph(font, c);
    float2 UVposition = float2(glyph->minX, glyph->minY) / 1024.0f;
    float2 UVsize = float2(abs(glyph->minX - glyph->maxX), abs(glyph->minY - glyph->maxY)) / 1024.0f;

    float2 pivotUV = float2(glyph->pivotX, glyph->pivotY) / 1024.0f;
    float2 advanceUV = float2(glyph->advanceX, glyph->advanceY) / 1024.0f;
    float2 advanceDelta = advanceUV - pivotUV;

    float2 PivotOffset = (UVposition - pivotUV) * scale;
    *pos += advanceDelta * scale;
    return true;
}
float GetLineWidth(char* text2, Font* font, float2 pos, float2 startPos, float scale, float maxWidth)
{
    // scan forward until end of line or end of string to figure out how long it is.
    float2 pos2 = pos;

    while (true)
    {
        IncrementGlyph(*text2, font, &pos2, startPos, scale);
        text2++;
        bool endOfString = *text2 == 0;
        bool endOfLine = *text2 == '\n';
        bool overrunMaxWidth = (abs(pos2.x - pos.x) > maxWidth) && (*text2 == ' ');
        if (endOfString)
            break;
        if (endOfLine)
            break;
        if (overrunMaxWidth)
            break;
        text2 += EscapeSequenceBold(text2);
        text2 += EscapeSequenceItalic(text2);
        text2 += EscapeSequenceColor(text2);
    }
    return pos2.x - pos.x;
}
// automatic reflowing
// adjust left/up/down/middle/etc
void DrawFontMain(char* text, Transform transform, float inScale, float maxLineWidth, HAlign hAlign, VAlign vAlign, bool screenspace, bool faceCamera)
{
    float3 color = { 1, 1, 1 };
    float opacity = 1;
    float scale = inScale;
    float2 startPos = float2(0, 0);
    
    if(screenspace)
        startPos = float2(transform.position.x, transform.position.y);

    bool debug = false;
    int lineCount = 0;
    for (int i = 0; i < StringLength(text); i++)
    {
        if (text[i] == '\n')
            lineCount++;
    }
    Font* currentFont = assets->FontDataRegular;

    float2 pos = startPos;
    float2 maxPos = pos;
    float currentLineWidth = 0;
    for (int i = 0; i < StringLength(text); i++)
    {
        bool newline = IncrementGlyph(text[i], currentFont, &pos, startPos, scale);
        if ((((pos.x - startPos.x) > maxLineWidth) && (text[i] == ' ')))
        {
            pos += float2(0.0f, newlineHeight * scale);
            pos.x = startPos.x;
            currentLineWidth = 0;
        }
        maxPos = max(pos, maxPos);
    }

    float2 boxSize = maxPos - startPos;

    float lineWidth = GetLineWidth(text, currentFont, pos, startPos, scale, maxLineWidth);
    float2 alignmentOffset = float2(0,0);

    if (vAlign == VAlign_up)
        alignmentOffset.y = -boxSize.y;
    if (vAlign == VAlign_center)
        alignmentOffset.y = -boxSize.y * 0.5;

    if (hAlign == HAlign_left)
        alignmentOffset.x = -boxSize.x;
    else if (hAlign == HAlign_center)
        alignmentOffset.x = -boxSize.x * 0.5;

    pos = startPos;
    if(debug)
        DrawBox2D(startPos + float2(alignmentOffset.x, alignmentOffset.y), boxSize, float3(0, 0, 0), 0.2);
    if (hAlign == HAlign_left)
        alignmentOffset.x = -lineWidth;
    else if (hAlign == HAlign_center)
        alignmentOffset.x = -lineWidth * 0.5;
    if (debug)
        DrawBox2D(pos + alignmentOffset, float2(lineWidth, 1));

    currentLineWidth = 0;
    bool bold = false;
    bool italic = false;
    for (int i = 0; i < StringLength(text); i++)
    {

        if (EscapeSequenceBold(&text[i]))
        {
            int escapeCount = EscapeSequenceBold(&text[i]);
            bold = !bold;
            if (bold)
                currentFont = assets->FontDataBold;
            else
                currentFont = assets->FontDataRegular;

            i += escapeCount - 1;
            continue;
        }

        if (EscapeSequenceItalic(&text[i]))
        {
            int escapeCount = EscapeSequenceItalic(&text[i]);
            italic = !italic;
            if (italic)
                currentFont = assets->FontDataItalic;
            else
                currentFont = assets->FontDataRegular;

            i += escapeCount - 1;
            continue;
        }

        if (EscapeSequenceColor(&text[i]))
        {
            int escapeCount = EscapeSequenceColor(&text[i]);

            if (escapeCount == 10) // change all
            {
                float red   = CharToNumber(text[i + 1]) * 16 + CharToNumber(text[i + 2]);
                float green = CharToNumber(text[i + 3]) * 16 + CharToNumber(text[i + 4]);
                float blue = CharToNumber(text[i + 5]) * 16 + CharToNumber(text[i + 6]);
                float alpha = CharToNumber(text[i + 7]) * 16 + CharToNumber(text[i + 8]);
                color = float3(red, green, blue) / 255.0f;
                opacity = alpha / 255.0f;
            }
            else if (escapeCount == 8) // change color only
            {
                float red   = CharToNumber(text[i + 1]) * 16 + CharToNumber(text[i + 2]);
                float green = CharToNumber(text[i + 3]) * 16 + CharToNumber(text[i + 4]);
                float blue  = CharToNumber(text[i + 5]) * 16 + CharToNumber(text[i + 6]);
                color = float3(red, green, blue) / 255.0f;
            }
            else if(escapeCount == 4) // change alpha only
            {
                float alpha = CharToNumber(text[i + 1]) * 16 + CharToNumber(text[i + 2]);
                opacity = alpha / 255.0f;
            }
            i += escapeCount - 1;
            continue;
        }

        bool endOfLine = text[i] == '\n';
        bool overrunMaxWidth = ((currentLineWidth > maxLineWidth) && (text[i] == ' '));
        if (endOfLine || overrunMaxWidth)
        {
            pos += float2(0.0f, newlineHeight * scale);
            pos.x = startPos.x;
            currentLineWidth = 0;
            lineWidth = GetLineWidth(text + i + 1, currentFont, pos, startPos, scale, maxLineWidth);
            if (hAlign == HAlign_left)
                alignmentOffset.x = -lineWidth;
            else if (hAlign == HAlign_center)
                alignmentOffset.x = -lineWidth * 0.5;
            if (debug)
                DrawBox2D(pos + alignmentOffset, float2(lineWidth, 1));
            continue;
        }
        if (text[i] == ' ')
        {
            pos += float2(spaceWidth * scale, 0.0f);
            currentLineWidth += spaceWidth * scale;
            continue;
        }

        Glyph* glyph = GetGlyph(currentFont, text[i]);
        float2 UVposition = float2(glyph->minX, glyph->minY) / 1024.0f;
        float2 UVsize = float2(abs(glyph->minX - glyph->maxX), abs(glyph->minY - glyph->maxY)) / 1024.0f;

        float2 pivotUV = float2(glyph->pivotX, glyph->pivotY) / 1024.0f;
        float2 advanceUV = float2(glyph->advanceX, glyph->advanceY) / 1024.0f;
        float2 advanceDelta = advanceUV - pivotUV;
        
        float2 PivotOffset = (UVposition - pivotUV) * scale;

        PivotOffset += alignmentOffset;


        int textureIndex = 1;
        if (currentFont == assets->FontDataBold)
            textureIndex = 2;
        if (currentFont == assets->FontDataItalic)
            textureIndex = 3;
        if (screenspace)
        {
            AppendQuadToUI(pos + PivotOffset, UVsize * scale, UVposition, UVsize, color, opacity, textureIndex);
        }
        else
        {
            if (faceCamera)
            {
                AppendQuadToUI3DCameraFacing(transform.position,
                    float3(pos.x, 0, pos.y) + float3(PivotOffset.x, 0, PivotOffset.y),
                    float3(UVsize.x, 0, UVsize.y) * scale,
                    UVposition,
                    UVsize, color, opacity, textureIndex);
            }
            else 
            {
                float2 p = pos + PivotOffset;
                float2 s = UVsize * scale;

                float3 pos0 = transform.position + transform.right * (p.x) - transform.up * (p.y) + float3(0,   0,   0);
                float3 pos1 = transform.position + transform.right * (p.x) - transform.up * (p.y) + float3(s.x, 0,   0);
                float3 pos2 = transform.position + transform.right * (p.x) - transform.up * (p.y) + float3(s.x, 0, -s.y);
                float3 pos3 = transform.position + transform.right * (p.x) - transform.up * (p.y) + float3(0,   0, -s.y);

                float2 uv0 = UVposition + float2(0, 0) * UVsize;
                float2 uv1 = UVposition + float2(1, 0) * UVsize;
                float2 uv2 = UVposition + float2(1, 1) * UVsize;
                float2 uv3 = UVposition + float2(0, 1) * UVsize;
                AppendSquareShapetoUI3D(pos0, pos1, pos2, pos3, uv0, uv1, uv2, uv3, color, opacity, textureIndex);

            }
        }

        

        pos += advanceDelta * scale;
        currentLineWidth += advanceDelta.x * scale;

    }
    if (debug)
    {
        DrawBox2D(startPos - float2(50, 3) * 0.5, float2(50, 3), float3(0.7, 0.2, 0.2));
        DrawBox2D(startPos - float2(3, 50) * 0.5, float2(3, 50), float3(0.2, 0.7, 0.2));
    }
}
void DrawFont2D(char* text, float2 position, float scale, float maxLineWidth, HAlign hAlign, VAlign vAlign)
{
    DrawFontMain(text, transform(float3(position.x, position.y, 0)), scale, maxLineWidth, hAlign, vAlign, true, false);
}
void DrawFont(char* text, Transform transform, float scale, float maxLineWidth, HAlign hAlign, VAlign vAlign)
{
    DrawFontMain(text, transform, scale, maxLineWidth, hAlign, vAlign, false, false);
}
void DrawFontCameraFacing(char* text, float3 position, float scale, float maxLineWidth, HAlign hAlign, VAlign vAlign)
{
    DrawFontMain(text, transform(position), scale, maxLineWidth, hAlign, vAlign, false, true);
}

float2 GetTextSize(char* text)
{
    int len = StringLength(text);
    int height = StringCountOccurances(text, '\n');
    return float2(len, height + 1) * fontSpriteSize * fontScale;
}


bool DrawToggle(char* text, float2* position, bool value)
{
    char wat[100] = "[";
    StringAppend(wat, text);
    StringAppend(wat, value);
    StringAppend(wat, "]");
    float2 textSize = GetTextSize(wat);

    if (input->mousePos.x > position->x && input->mousePos.y > position->y &&
        input->mousePos.x < position->x + textSize.x && input->mousePos.y < position->y + textSize.y)
    {
        DrawText(wat, position, float3(0.75, 0.75, 0.75));
        if (input->mouseLeftDown)
        {
            return !value;
        }
    }
    else
    {
        DrawText(wat, position, float3(1, 1, 1));
    }

    return value;
}
bool DrawToggle(char* text, bool value)
{
    return DrawToggle(text, &haven->uiPos, value);
}

bool DrawButton(char* text, float2* position)
{
    char wat[100] = "[";
    StringAppend(wat, text);
    StringAppend(wat, "]");
    float2 textSize = GetTextSize(wat);

    if (input->mousePos.x > position->x && input->mousePos.y > position->y &&
        input->mousePos.x < position->x + textSize.x && input->mousePos.y < position->y + textSize.y)
    {
        DrawText(wat, position, float3(0.75, 0.75, 0.75));
        if (input->mouseLeftDown)
        {
            return true;
        }
    }
    else
    {
        DrawText(wat, position, float3(1, 1, 1));
    }

    return false;
}
bool DrawButton(char* text)
{
    return DrawButton(text, &haven->uiPos);
}

void DrawLine(float3 start, float3 end, float width = 0.01f, float3 color = {1, 1, 1}, float opacity = 1)
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

    AppendSquareShapetoUI3D(pos0, pos1, pos2, pos3, color, opacity);

    pos0 = end + right;
    pos1 = start + right;
    pos2 = start - right;
    pos3 = end - right;
    AppendSquareShapetoUI3D(pos0, pos1, pos2, pos3, color, opacity);

}

void DrawRay(float3 start, float3 direction, float len, float width = 0.01f, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawLine(start, start + normalize(direction) * len, width, color, opacity);
}

float3 AngleToVector(float angle, float3 left, float3 right)
{
    return sinTurns(angle) * left + cosTurns(angle) * right;
}

void DrawCircle(float3 center, float3 normal, float radius, float width = 0.01, float3 color = { 1, 1, 1 }, float opacity = 1, int detail = 32)
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
        AppendSquareShapetoUI3D(pos0, pos1, pos2, pos3, color, opacity);

        pos0 = center + offsetStart * radius - direction * width;
        pos1 = center + offsetEnd   * radius - direction * width;
        pos2 = center + offsetEnd   * radius + direction * width;
        pos3 = center + offsetStart * radius + direction * width;
        AppendSquareShapetoUI3D(pos0, pos1, pos2, pos3, color, opacity);
    }
}

void DrawSphere(float3 center, float radius, float width = 0.01, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    for (int i = 0; i < 8; i++)
    {
        float3 a = AngleToVector(i / 8.0f, float3(1, 0, 0), float3(0, 1, 0));
        DrawCircle(center, a, radius, width, color, opacity, 8);
    }
    DrawCircle(center + float3(0, 0, radius * 0.707), float3(0, 0, 1), radius* 0.707, width, color, opacity, 8);
    DrawCircle(center, float3(0, 0, 1), radius, width, color, opacity, 8);
    DrawCircle(center + float3(0, 0, -radius * 0.707), float3(0, 0, 1), radius * 0.707, width, color, opacity, 8);
}

void DrawAxisSphere(float3 center, float radius, float width = 0.01, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawCircle(center, float3(1, 0, 0), radius, width, color, opacity, 24);
    DrawCircle(center, float3(0, 1, 0), radius, width, color, opacity, 24);
    DrawCircle(center, float3(0, 0, 1), radius, width, color, opacity, 24);
}

void DrawTransform(Transform transform, float width = 0.01)
{
    DrawRay(transform.position, transform.right,   transform.scale.x, width, float3(1, 0, 0));
    DrawRay(transform.position, transform.forward, transform.scale.y, width, float3(0, 1, 0));
    DrawRay(transform.position, transform.up,      transform.scale.z, width, float3(0, 0, 1));
}

void DrawPoint(float3 position, float width = 0.01, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawLine(position - float3(width * 0.5, 0, 0), position + float3(width * 0.5, 0, 0), width, color, opacity);
    DrawLine(position - float3(0, width * 0.5, 0), position + float3(0, width * 0.5, 0), width, color, opacity);
}

void DrawAABB(float3 center, float3 size, float width = 0.01, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawLine(center + size * float3( 1,  1,  1), center + size * float3( 1,  1, -1), width, color, opacity);
    DrawLine(center + size * float3(-1,  1,  1), center + size * float3(-1,  1, -1), width, color, opacity);
    DrawLine(center + size * float3( 1, -1,  1), center + size * float3( 1, -1, -1), width, color, opacity);
    DrawLine(center + size * float3(-1, -1,  1), center + size * float3(-1, -1, -1), width, color, opacity);

    DrawLine(center + size * float3( 1,  1,  1), center + size * float3( 1, -1,  1), width, color, opacity);
    DrawLine(center + size * float3(-1,  1,  1), center + size * float3(-1, -1,  1), width, color, opacity);
    DrawLine(center + size * float3( 1,  1, -1), center + size * float3( 1, -1, -1), width, color, opacity);
    DrawLine(center + size * float3(-1,  1, -1), center + size * float3(-1, -1, -1), width, color, opacity);

    DrawLine(center + size * float3(1,  1,   1), center + size * float3(-1,  1,  1), width, color, opacity);
    DrawLine(center + size * float3(1, -1,   1), center + size * float3(-1, -1,  1), width, color, opacity);
    DrawLine(center + size * float3(1,  1,  -1), center + size * float3(-1,  1, -1), width, color, opacity);
    DrawLine(center + size * float3(1, -1,  -1), center + size * float3(-1, -1, -1), width, color, opacity);
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

void DrawBox(Transform transform, float width = 0.01, float3 color = { 1, 1, 1 }, float opacity = 1)
{
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1, -1)), width, color, opacity);
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1, -1)), width, color, opacity);
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1, -1)), width, color, opacity);
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1, -1)), width, color, opacity);
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1,  1)), width, color, opacity);
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1,  1)), width, color, opacity);
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1, -1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1, -1)), width, color, opacity);
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1, -1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1, -1)), width, color, opacity);
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1,  1)), width, color, opacity);
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1,  1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1,  1)), width, color, opacity);
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1,  1, -1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1,  1, -1)), width, color, opacity);
    DrawLine(TransformPosition_LocalSpaceToWorldSpace(transform, float3( 1, -1, -1)), TransformPosition_LocalSpaceToWorldSpace(transform, float3(-1, -1, -1)), width, color, opacity);
}
