
#pragma pack(push, 1)
struct TGA_HEADER {
    uint8 idlength;
    uint8 colourmaptype;
    uint8 datatypecode;
    int16 colourmaporigin;
    int16 colourmaplength;
    uint8 colourmapdepth;
    int16 x_origin;
    int16 y_origin;
    uint16 width;
    uint16 height;
    uint8 bitsperpixel;
    uint8 imagedescriptor;
};
#pragma pack(pop)

void LoadTexture(EngineState* engineState, Texture* texture)
{
    bool read = engineState->platformReadFile(engineState->scratchBuffer, texture->filename);
    
    // File failed to read!
    Assert(read);

    TGA_HEADER* tga = (TGA_HEADER*)engineState->scratchBuffer;

    texture->data = (uint8*)ArenaPushBytes(&engineState->arenaHotreload, tga->width * tga->height * 4, texture->filename);
    texture->width = tga->width;
    texture->height = tga->height;

    for (int i = 0; i < (tga->width * tga->height); i++)
    {
        // Swap R and B
        texture->data[i * 4 + 2] = 0;
        texture->data[i * 4 + 1] = 0;
        texture->data[i * 4 + 0] = 0;
        texture->data[i * 4 + 3] = 0;
        texture->data[i * 4 + 2] = *((uint8*)tga + sizeof(TGA_HEADER) + i * 4 + 0);
        texture->data[i * 4 + 1] = *((uint8*)tga + sizeof(TGA_HEADER) + i * 4 + 1);
        texture->data[i * 4 + 0] = *((uint8*)tga + sizeof(TGA_HEADER) + i * 4 + 2);
        texture->data[i * 4 + 3] = *((uint8*)tga + sizeof(TGA_HEADER) + i * 4 + 3);
    }

    texture->GLID = engineState->platformGraphicsLoadTexture(texture);
}

Texture* FileReadTexture(EngineState* engineState, const char* filename)
{
    Texture* texture = ArrayAddNew(engineState->textures);
    StringCopy(texture->filename, filename);
    LoadTexture(engineState, texture);
    return texture;
}

