#pragma once
#include "haven.cpp"
#include "entities/ent_lightbaker.h"

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
    uint16 sizeX;
    uint16 sizeY;
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

void LoadASTC(Texture* texture, int fileSize)
{
    texture->ASTC = true;
    texture->HDR = false;

    uint8* currentMip = haven->scratchBuffer;

    // copy the raw bytes
    int i = 0;
    while (isASTC(currentMip))
    {
        unpackedASTC astc = unpackASTCHeader(currentMip);
        if (i == 0)
        {
            texture->sizeX = astc.sizeX;
            texture->sizeY = astc.sizeY;
            texture->size.x = texture->sizeX;
            texture->size.y = texture->sizeY;
        }
        texture->mipSizeX[i] = astc.sizeX;
        texture->mipSizeY[i] = astc.sizeY;

        texture->mips[i] = (uint8*)ArenaPushBytes(&haven->arenaAssets, astc.dataSize, texture->filename);
        Copy(astc.dataStart, texture->mips[i], astc.dataSize);
        texture->mipSize[i] = astc.dataSize;

        i++;
        currentMip += sizeof(ASTC_HEADER) + astc.dataSize;
    }

    texture->mipCount = i;
    texture->GLID = haven->platformGraphicsLoadTexture(texture, false, false);
}

void LoadTga(Texture* texture)
{
    TGA_HEADER* tga = (TGA_HEADER*)haven->scratchBuffer;
    bool flipUpDown = tga->imagedescriptor & 1 << 5;
    bool flipLeftRight = tga->imagedescriptor & 1 << 4;

    int ChannelCount = 4;
    int BytesPerChannel = 1;
    texture->mipSize[0] = tga->sizeX * tga->sizeY * ChannelCount * BytesPerChannel;

    texture->mips[0] = (uint8*)ArenaPushBytes(&haven->arenaAssets, texture->mipSize[0], texture->filename);
    texture->sizeX = tga->sizeX;
    texture->sizeY = tga->sizeY;
    texture->size = float2(tga->sizeX, tga->sizeY);
    texture->mipSizeX[0] = tga->sizeX;
    texture->mipSizeY[0] = tga->sizeY;
    texture->ASTC           = false;
    texture->HDR            = false;


    ProfilerBeingSample();
    if (flipUpDown && flipLeftRight)
    {
        int i = 0;
        for (int y = tga->sizeY - 1; y >= 0; y--)
        {
            for (int x = tga->sizeX - 1; x >= 0; x--)
            {
                //int j = x + y * tga->width;
                //uint32* source = (uint32*)(((uint8*)tga + sizeof(TGA_HEADER) + j * 4));
                //uint32* target = (uint32*)(&texture->mips[0][i * 4]);
                //*source = *target;
                //i++;

                int j = x + y * tga->sizeX;
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
        for (int y = tga->sizeY - 1; y >= 0; y--)
        {
            //uint8* destination = texture->mips[0] + tga->width * y * 4;
            //uint8* source = (uint8*)tga + sizeof(TGA_HEADER) + tga->width * (tga->height - y) * 4;
            //Copy(source, destination, tga->width * 4);
            for (int x = 0; x < tga->sizeX; x++)
            {
                //int i = x + y * tga->width;
                int j = x + y * tga->sizeX;
                texture->mips[0][i * 4 + 0] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 0);
                texture->mips[0][i * 4 + 1] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 1);
                texture->mips[0][i * 4 + 2] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 2);
                texture->mips[0][i * 4 + 3] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 3);
                i++;
            }
        }
    }
    else if (flipLeftRight)
    {
        int i = 0;
        for (int y = 0; y < tga->sizeY; y++)
        {
            for (int x = tga->sizeX - 1; x >= 0; x--)
            {
                //int i = x + y * tga->width;
                int j = x + y * tga->sizeX;
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
        for (int y = 0; y < tga->sizeY; y++)
        {
            for (int x = 0; x < tga->sizeX; x++)
            {
                //int i = x + y * tga->width;
                int j = x + y * tga->sizeX;
                texture->mips[0][i * 4 + 0] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 0);
                texture->mips[0][i * 4 + 1] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 1);
                texture->mips[0][i * 4 + 2] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 2);
                texture->mips[0][i * 4 + 3] = *((uint8*)tga + sizeof(TGA_HEADER) + j * 4 + 3);
                i++;
            }
        }
    }
    ProfilerEndSample("Copy");

    ProfilerBeingSample();
    texture->GLID = haven->platformGraphicsLoadTexture(texture, false, true);
    ProfilerEndSample("Upload");
}

// load "radiosity" texture (voxel array of baked lighting data)
void LoadRad(Texture* texture, int fileSize)
{
    uint64 sizeX = ((uint64*)haven->scratchBuffer)[0];//radiosityTextureSizeX;
    uint64 sizeY = ((uint64*)haven->scratchBuffer)[1];//radiosityTextureSizeY;
    int ChannelCount = 4;
    int BytesPerChannel = 4;
    texture->mipSize[0] = sizeX * sizeY * ChannelCount * BytesPerChannel;

    ProfilerBeingSample();
    texture->mips[0] = (uint8*)ArenaPushBytes(&haven->arenaAssets, texture->mipSize[0], texture->filename);
    texture->sizeX = sizeX;
    texture->sizeY = sizeY;
    texture->size = float2(sizeX, sizeY);
    texture->mipSizeX[0] = sizeX;
    texture->mipSizeY[0] = sizeY;
    texture->ASTC = false;
    texture->HDR = true;
    Copy(haven->scratchBuffer + sizeof(uint64)*2, texture->mips[0], texture->mipSize[0]);
    ProfilerEndSample("Copy");

    ProfilerBeingSample();
    texture->GLID = haven->platformGraphicsLoadTexture(texture, true, false);
    ProfilerEndSample("Upload");
}

void LoadTexture(Texture* texture)
{
    ProfilerBeingSample();
    uint8* end = haven->platformReadFile(haven->scratchBuffer, texture->filename);
    ProfilerEndSample("Read file from disk.");

    // failed to read texture, return the 'missing' texture.
    if (!end)
    {
        Texture* missing        = assets->missingTexture;
        texture->sizeX          = missing->sizeX;
        texture->sizeY          = missing->sizeY;
        texture->size.x = texture->sizeX;
        texture->size.y = texture->sizeY;
        texture->mips[0]        = missing->mips[0];
        texture->mipCount       = missing->mipCount;
        texture->mipSizeX[0]    = missing->mipSizeX[0];
        texture->mipSizeY[0]    = missing->mipSizeY[0];
        texture->GLID           = missing->GLID;
        texture->ASTC           = false;
        texture->HDR            = false;
        return;
    }
    // Clear the mip data
    texture->mipCount = 0;
    for (int i = 0; i < ArrayCapacity(texture->mips); i++)
    {
        texture->mips[i] = 0;
        texture->mipSize[i] = 0;
    }

    // Check if it's an ASTC compressed texture (android). If it is, load it that way.
    if (isASTC(haven->scratchBuffer))
    {
        LoadASTC(texture, end - haven->scratchBuffer);
        return;
    }

    // Check if it's a .tga targa file. If it is, load it that way.
    if (EndsWith(texture->filename, ".tga"))
    {
        LoadTga(texture);
        return;
    }

    if (EndsWith(texture->filename, ".rad"))
    {
        LoadRad(texture, end - haven->scratchBuffer);
        return;
    }

    Assert(false, "Failed to figure out what kind of texture this is.");

}

Texture* FileReadTexture(const char* filename)
{
    ProfilerBeingSample();
    Texture* texture = ArrayAddNew(haven->globalTextures);
    StringCopy(texture->filename, filename);
    LoadTexture(texture);
    ProfilerEndSample("LoadTexture");
    return texture;
}
