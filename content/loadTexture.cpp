#pragma once
#include "haven.cpp"

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
struct ASTC_HEADER
{
    uint8 magic[4];
    uint8 block_x;
    uint8 block_y;
    uint8 block_z;
    uint8 dim_x[3];
    uint8 dim_y[3];
    uint8 dim_z[3];
};
#pragma pack(pop)

void LoadASTC(EngineState* engineState, Texture* texture, int fileSize)
{
    ASTC_HEADER* astc = (ASTC_HEADER*)engineState->scratchBuffer;
    texture->ASTC = true;

    texture->width = astc->dim_x[0] + (astc->dim_x[1] << 8) + (astc->dim_x[2] << 16);
    texture->height = astc->dim_y[0] + (astc->dim_y[1] << 8) + (astc->dim_y[2] << 16);

    int xdim = astc->block_x;
    int ydim = astc->block_y;
    int zdim = astc->block_z;
    int xsize = astc->dim_x[0] + 256 * astc->dim_x[1] + 65536 * astc->dim_x[2];
    int ysize = astc->dim_y[0] + 256 * astc->dim_y[1] + 65536 * astc->dim_y[2];
    int zsize = astc->dim_z[0] + 256 * astc->dim_z[1] + 65536 * astc->dim_z[2];
    int xblocks = (xsize + xdim - 1) / xdim;
    int yblocks = (ysize + ydim - 1) / ydim;
    int zblocks = (zsize + zdim - 1) / zdim;
    int size = xblocks * yblocks * zblocks * 16;

    int size2 = ceil(xsize / 8.0f) * ceil(ysize / 8.0f) * 16;

    texture->width = xsize;
    texture->height = ysize;

    texture->dataSize = size;
    texture->data = (uint8*)ArenaPushBytes(&engineState->arenaHotreload, texture->dataSize, texture->filename);

    // copy the raw bytes
    Copy((uint8*)(astc + 1), texture->data, texture->dataSize);

    texture->GLID = engineState->platformGraphicsLoadTexture(texture);

}

void LoadTexture(EngineState* engineState, Texture* texture)
{
    uint8* end = engineState->platformReadFile(engineState->scratchBuffer, texture->filename);
    
    // File failed to read!
    Assert(end);

    // Check if it's an ASTC compressed texture (android). If it is, load it that way.
    ASTC_HEADER* astc = (ASTC_HEADER*)engineState->scratchBuffer;
    if (astc->magic[0] == 0x13 &&
        astc->magic[1] == 0xAB &&
        astc->magic[2] == 0xA1 &&
        astc->magic[3] == 0x5C)
    {
        LoadASTC(engineState, texture, end - engineState->scratchBuffer);
        return;
    }

    TGA_HEADER* tga = (TGA_HEADER*)engineState->scratchBuffer;
    bool flipUpDown = tga->imagedescriptor & 1 << 5;
    bool flipLeftRight = tga->imagedescriptor & 1 << 4;

    texture->dataSize = tga->width * tga->height * 4;
    texture->data = (uint8*)ArenaPushBytes(&engineState->arenaHotreload, texture->dataSize, texture->filename);
    texture->width = tga->width;
    texture->height = tga->height;

    for (int y = 0; y < tga->height; y++)
    {
        for (int x = 0; x < tga->width; x++)
        {
            int i = x + y * tga->width;
            int j = x + y * tga->width;
            if(flipLeftRight && flipUpDown)
                j = (tga->width - x - 1) + (tga->height - y - 1) * tga->width;
            else if (flipLeftRight)
                j = (tga->width - x - 1) + y * tga->width;
            else if(flipUpDown)
                j = x + (tga->height - y - 1) * tga->width;

            texture->data[i * 4 + 2] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 0);
            texture->data[i * 4 + 1] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 1);
            texture->data[i * 4 + 0] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 2);
            texture->data[i * 4 + 3] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 3);
        }
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

