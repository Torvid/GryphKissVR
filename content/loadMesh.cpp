#pragma once

#include "string.cpp"

bool ObjIsDigit(char c)
{
    return c == '0' ||
        c == '1' ||
        c == '2' ||
        c == '3' ||
        c == '4' ||
        c == '5' ||
        c == '6' ||
        c == '7' ||
        c == '8' ||
        c == '9';
}

// Parses a float and moves the char pointer forward past it.
float ObjParseFloat(char** stringPtr)
{
    char* string = *stringPtr;

    // read forward until we find a relevant character
    while (!ObjIsDigit(*string) &&
        *string != '.' &&
        *string != '-') {
        string++;
    }

    // Check if it's negative.
    bool negative = *string == '-';
    if (negative)
        string++;

    float result = 0;

    // read forward until we hit the '.'
    int intigerCount = 0;
    while (*string != '.')
    {
        intigerCount++;
        string++;
    }

    // Step backwards character by character, multiplying by 10 each step
    float multiplier = 1;
    for (int i = 0; i < intigerCount; i++)
    {
        string--;
        result += ((*string) - '0') * multiplier;
        multiplier *= 10;
    }

    string += intigerCount + 1;
    // read forward until we run out of digits
    multiplier = 0.1f;
    while (ObjIsDigit(*string))
    {
        result += ((*string) - '0') * multiplier;
        multiplier *= 0.1f;
        string++;
    }

    //string -= intigerCount + decmialCount;

    if (negative)
        result *= -1;

    *stringPtr = string;
    return result;
}

int ObjParseInt(char** stringPtr)
{
    char* string = *stringPtr;
    // scan forward until we find a digit
    while (!ObjIsDigit(*string)) { string++; };

    // read forward until we hit the '.'
    int intigerCount = 0;
    while (ObjIsDigit(*string))
    {
        intigerCount++;
        string++;
    }

    int result = 0;
    // Step backwards character by character, multiplying by 10 each step
    int multiplier = 1;
    for (int i = 0; i < intigerCount; i++)
    {
        string--;
        result += ((*string) - '0') * multiplier;
        multiplier *= 10;
    }

    *stringPtr = string + intigerCount;
    return result;
}

enum ObjLineType
{
    ObjLineType_None,
    ObjLineType_Position,
    ObjLineType_Normal,
    ObjLineType_UV,
    ObjLineType_Face
};
ObjLineType ObjClassifyLine(char* line)
{
    if (line[0] == 'v' && line[1] == ' ')
        return ObjLineType_Position;
    if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
        return ObjLineType_Normal;
    if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
        return ObjLineType_UV;
    if (line[0] == 'f')
        return ObjLineType_Face;

    return ObjLineType_None;

}
char* GoToNextLineOrEndOfString(char* text)
{
    // If the string starts with a newline, skip over it.
    while ((*text != '\n') && (*text != 0))
    {
        text++;
    }

    if (*text == '\n')
        text++;

    return text;
}

void LoadObj(EngineState* engineState, Mesh* mesh)
{
    int i = sizeof(Mesh);
    uint8* moreScratchBuffer = engineState->platformReadFile(engineState->scratchBuffer, mesh->filename);

    // Failed to load file, wrong filename?
    Assert(moreScratchBuffer);

    // Read all the data from the text file.
    ObjMeshTemp* temp = (ObjMeshTemp*)moreScratchBuffer;
    int positionCount = 0;
    int normalCount = 0;
    int uvCount = 0;
    int indexCount = 0;
    char* text = (char*)engineState->scratchBuffer;
    while (*text != 0)
    {
        ObjLineType lineType = ObjClassifyLine(text);
        switch (lineType)
        {
        case ObjLineType_Position:
        {
            float thing1 = ObjParseFloat(&text);
            float thing2 = ObjParseFloat(&text);
            float thing3 = ObjParseFloat(&text);
            temp->positions[positionCount] = { thing1, thing2, thing3 };
            positionCount++;
        }
        break;
        case ObjLineType_Normal:
            temp->normals[normalCount] = {
                ObjParseFloat(&text),
                ObjParseFloat(&text),
                ObjParseFloat(&text) };
            normalCount++;
            break;
        case ObjLineType_UV:
            temp->uvs[uvCount] = {
                ObjParseFloat(&text),
                ObjParseFloat(&text) };
            uvCount++;
            break;
        case ObjLineType_Face:
            for (int i = 0; i < 9; i++)
            {
                temp->indexes[indexCount] = ObjParseInt(&text);
                indexCount++;
            }

            break;
        default:
            break;
        }
        text = GoToNextLineOrEndOfString(text);
    }

    mesh->vertexCount = indexCount / 3;
    mesh->indexCount = indexCount / 3;

    mesh->vertexes = ArenaPushArray(&engineState->arenaHotreload, mesh->vertexCount, Vertex, mesh->filename);
    mesh->indexes = ArenaPushArray(&engineState->arenaHotreload, mesh->indexCount, uint32, mesh->filename);

    for (int i = 0; i < mesh->vertexCount; i++)
    {
        int pos = temp->indexes[(i * 3) + 0] - 1;
        int uv = temp->indexes[(i * 3) + 1] - 1;
        int norm = temp->indexes[(i * 3) + 2] - 1;

        mesh->vertexes[i].position = temp->positions[pos];
        mesh->vertexes[i].normal = temp->normals[norm];
        mesh->vertexes[i].uv0 = temp->uvs[uv];
        mesh->vertexes[i].materialID = 0;
        mesh->indexes[i] = i;
    }

    mesh->GLID = engineState->platformGraphicsLoadMesh(mesh);

}

struct MESH_HEADER
{
    char magicCookie[16];
    uint32 indexCount;
    uint32 vertexCount;
    uint32 skinned; // bool
};

struct BoneMetadata
{
    char name[100];
    int32 parent;
};

struct BONE_DATA_HEADER
{
    uint32 boneCount;
    uint32 frameCount;
};

void LoadMesh(EngineState* engineState, Mesh* mesh)
{
    if (EndsWith(mesh->filename, ".obj"))
    {
        LoadObj(engineState, mesh);
        return;
    }

    // Push file into the hoterload arena
    MESH_HEADER* header = (MESH_HEADER*)(engineState->arenaHotreload.base + engineState->arenaHotreload.used);
    uint8* end = (uint8*)engineState->platformReadFile((uint8*)header, mesh->filename);
    
    Assert(end, "File not found");
    
    ArenaPushBytes(&engineState->arenaHotreload, (uint8*)end - (uint8*)header, mesh->filename);

    // Crash if we read the wrong file
    Assert(Equals((uint8*)header->magicCookie, (uint8*)"h a v e n - mesh", 16));

    // get metadata
    mesh->indexCount = header->indexCount;
    mesh->vertexCount = header->vertexCount;
    mesh->skinned = header->skinned;

    mesh->indexes = (uint32*)(header + 1);
    mesh->vertexes = (Vertex*)(mesh->indexes + mesh->indexCount);

    if (mesh->skinned)
    {
        mesh->vertexes = 0;
        mesh->skinnedVertexes = (VertexSkinned*)(mesh->indexes + mesh->indexCount);
        BONE_DATA_HEADER* boneData = (BONE_DATA_HEADER*)(mesh->skinnedVertexes + mesh->vertexCount);
        BoneMetadata* metadatas = (BoneMetadata*)(boneData + 1);
        Transform* transforms = (Transform*)(metadatas + boneData->boneCount);

        mesh->boneCount = boneData->boneCount;
        Assert(mesh->boneCount < 200);

        //mesh->bp2 = transforms;
        //for (int i = 0; i < mesh->boneCount; i++)
        //{
        //    
        //}
        mesh->bindPose = transforms;

        for (int i = 0; i < mesh->boneCount; i++)
        {
            //mesh->bindPose[i] = {};
            mesh->boneHierarchy[i] = {};
        }
        for (int i = 0; i < mesh->boneCount; i++)
        {
            Bone* bone = &mesh->boneHierarchy[i];
            bone->index = i;
            bone->name = metadatas[i].name;
            
            if (metadatas[i].parent == -1)
            {
                bone->parent = 0;
            }
            else
            {
                // set our parent.
                bone->parent = &mesh->boneHierarchy[metadatas[i].parent];
                // set outselves as the child of the parent.
                ArrayAdd(bone->parent->children, bone);
            }
        }
    }
    
    // Upload to GPU
    mesh->GLID = engineState->platformGraphicsLoadMesh(mesh);
}

Mesh* FileReadMesh(EngineState* engineState, const char* filename)
{
    Mesh* mesh = ArrayAddNew(engineState->meshes);
    StringCopy(mesh->filename, filename);

    LoadMesh(engineState, mesh);
    
    return mesh;
}

void UpdateMesh(EngineState* engineState, Mesh* mesh, Vertex* vertexes, int vertexCount, uint32* indexes, int indexCount) // Updates the geometry of a mesh.
{
    mesh->vertexes = vertexes;
    mesh->vertexCount = vertexCount;
    mesh->indexes = indexes;
    mesh->indexCount = indexCount;
    mesh->filename[0] = {};
    mesh->GLID = engineState->platformGraphicsLoadMesh(mesh);
}
