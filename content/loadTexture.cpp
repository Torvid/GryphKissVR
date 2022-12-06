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

struct unpackedASTC
{
    int sizeX;
    int sizeY;
    ASTC_HEADER* header;
    uint8* dataStart;
    int dataSize;
};

bool isASTC(uint8* fileData)
{
    ASTC_HEADER* astc = (ASTC_HEADER*)fileData;
    return (astc->magic[0] == 0x13 &&
        astc->magic[1] == 0xAB &&
        astc->magic[2] == 0xA1 &&
        astc->magic[3] == 0x5C);
}

unpackedASTC unpackASTCHeader(uint8* fileData)
{
    unpackedASTC Result;
    ASTC_HEADER* astc = (ASTC_HEADER*)fileData;

    Assert(isASTC(fileData), "Error, not a valid ASTC file.");

    int xsize = astc->dim_x[0] + 256 * astc->dim_x[1] + 65536 * astc->dim_x[2];
    int ysize = astc->dim_y[0] + 256 * astc->dim_y[1] + 65536 * astc->dim_y[2];
    int zsize = astc->dim_z[0] + 256 * astc->dim_z[1] + 65536 * astc->dim_z[2];
    int xblocks = (xsize + astc->block_x - 1) / astc->block_x;
    int yblocks = (ysize + astc->block_y - 1) / astc->block_y;
    int zblocks = (zsize + astc->block_z - 1) / astc->block_z;


    Result.sizeX = xsize;// astc->dim_x[0] + (astc->dim_x[1] << 8) + (astc->dim_x[2] << 16);
    Result.sizeY = ysize;// astc->dim_y[0] + (astc->dim_y[1] << 8) + (astc->dim_y[2] << 16);
    Result.header = astc;

    Result.dataStart = (uint8*)(astc + 1);
    Result.dataSize = xblocks * yblocks * zblocks * 16;

    return Result;
}

void LoadASTC(EngineState* engineState, Texture* texture, int fileSize)
{
    texture->ASTC = true;

    uint8* currentMip = engineState->scratchBuffer;

    // copy the raw bytes
    int i = 0;
    while (isASTC(currentMip))
    {
        unpackedASTC astc = unpackASTCHeader(currentMip);
        if (i == 0)
        {
            texture->width = astc.sizeX;
            texture->height = astc.sizeY;
        }
        texture->mipSizeX[i] = astc.sizeX;
        texture->mipSizeY[i] = astc.sizeY;

        texture->mips[i] = (uint8*)ArenaPushBytes(&engineState->arenaHotreload, astc.dataSize, texture->filename);
        Copy(astc.dataStart, texture->mips[i], astc.dataSize);
        texture->mipSize[i] = astc.dataSize;

        i++;
        currentMip += sizeof(ASTC_HEADER) + astc.dataSize;
    }

    texture->mipCount = i;
    texture->GLID = engineState->platformGraphicsLoadTexture(texture);
}

void LoadTexture(EngineState* engineState, Texture* texture)
{
    ProfilerBeingSample(engineState);
    uint8* end = engineState->platformReadFile(engineState->scratchBuffer, texture->filename);
    ProfilerEndSample(engineState, "Read File");

    ProfilerBeingSample(engineState);
    if (!end)
    {
        Texture* missing        = assets->missingTexture;
        texture->width          = missing->width;
        texture->height         = missing->height;
        texture->mips[0]        = missing->mips[0];
        texture->mipCount       = missing->mipCount;
        texture->mipSizeX[0]    = missing->mipSizeX[0];
        texture->mipSizeY[0]    = missing->mipSizeY[0];
        texture->GLID           = missing->GLID;
        return;
    }
    // File failed to read!
    //Assert(end);

    texture->mipCount = 0;
    for (int i = 0; i < ArrayCapacity(texture->mips); i++)
    {
        texture->mips[i] = 0;
        texture->mipSize[i] = 0;
    }

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

    texture->mipSize[0] = tga->width * tga->height * 4;
    
    texture->mips[0] = (uint8*)ArenaPushBytes(&engineState->arenaHotreload, texture->mipSize[0], texture->filename);
    texture->width = tga->width;
    texture->height = tga->height;
    texture->mipSizeX[0] = tga->width;
    texture->mipSizeY[0] = tga->height;

    ProfilerEndSample(engineState, "Misc");

    ProfilerBeingSample(engineState);
    if (flipUpDown && flipLeftRight)
    {
        int i = 0;
        for (int y = tga->height - 1; y >= 0; y--)
        {
            for (int x = tga->width - 1; x >= 0; x--)
            {
                //int j = x + y * tga->width;
                //uint32* source = (uint32*)(((uint8*)tga + sizeof(TGA_HEADER) + j * 4));
                //uint32* target = (uint32*)(&texture->mips[0][i * 4]);
                //*source = *target;
                //i++;

                int j = x + y * tga->width;
                texture->mips[0][i * 4 + 0] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 0);
                texture->mips[0][i * 4 + 1] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 1);
                texture->mips[0][i * 4 + 2] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 2);
                texture->mips[0][i * 4 + 3] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 3);
                i++;
            }
        }
    }
    else if (flipUpDown)
    {
        int i = 0;
        for (int y = tga->height - 1; y >= 0; y--)
        {
            uint8* destination = texture->mips[0] + tga->width * y * 4;
            uint8* source = (uint8*)tga + sizeof(TGA_HEADER) + tga->width * (tga->height - y) * 4;
            Copy(source, destination, tga->width * 4);
            //for (int x = 0; x < tga->width; x++)
            //{
            //    //int i = x + y * tga->width;
            //    int j = x + y * tga->width;
            //    texture->mips[0][i * 4 + 0] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 0);
            //    texture->mips[0][i * 4 + 1] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 1);
            //    texture->mips[0][i * 4 + 2] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 2);
            //    texture->mips[0][i * 4 + 3] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 3);
            //    i++;
            //}
        }
    }
    else if (flipLeftRight)
    {
        int i = 0;
        for (int y = 0; y < tga->height; y++)
        {
            for (int x = tga->width - 1; x >= 0; x--)
            {
                //int i = x + y * tga->width;
                int j = x + y * tga->width;
                texture->mips[0][i * 4 + 0] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 0);
                texture->mips[0][i * 4 + 1] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 1);
                texture->mips[0][i * 4 + 2] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 2);
                texture->mips[0][i * 4 + 3] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 3);
                i++;
            }
        }
    }
    else
    {
        int i = 0;
        for (int y = 0; y < tga->height; y++)
        {
            for (int x = 0; x < tga->width; x++)
            {
                //int i = x + y * tga->width;
                int j = x + y * tga->width;
                texture->mips[0][i * 4 + 0] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 0);
                texture->mips[0][i * 4 + 1] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 1);
                texture->mips[0][i * 4 + 2] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 2);
                texture->mips[0][i * 4 + 3] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 3);
                i++;
            }
        }
    }
    ProfilerEndSample(engineState, "Copy");

    ProfilerBeingSample(engineState);
    texture->GLID = engineState->platformGraphicsLoadTexture(texture);
    ProfilerEndSample(engineState, "Upload");
}

Texture* FileReadTexture(EngineState* engineState, const char* filename)
{
    ProfilerBeingSample(engineState);
    Texture* texture = ArrayAddNew(engineState->textures);
    StringCopy(texture->filename, filename);
    LoadTexture(engineState, texture);
    ProfilerEndSample(engineState, "LoadTexture");
    return texture;
}

