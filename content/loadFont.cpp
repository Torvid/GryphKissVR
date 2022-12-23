#pragma once
#include "haven.cpp"

// SEX
void LoadFont(Font* font, const char* filename)
{
    haven->platformReadFile((uint8*)font, filename);
}

Font* FileReadFont(const char* filename)
{
    //ProfilerBeingSample();
    Font* font = ArrayAddNew(haven->fonts);
    //StringCopy(font->filename, filename);
    LoadFont(font, filename);
    //ProfilerEndSample("LoadFont");
    return font;
}
