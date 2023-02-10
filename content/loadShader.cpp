#pragma once
#include "haven.cpp"

char* ShaderReadFile(Shader* shader, char* destination, char* scratchBufferFilesDepth, char* filename, char* pixelShader, bool isPixelShader)
{
    char* fileStep = scratchBufferFilesDepth;
    char* fileStep2 = scratchBufferFilesDepth;
    char* fileStep3 = scratchBufferFilesDepth;
    scratchBufferFilesDepth = (char*)haven->platformReadFile((uint8*)scratchBufferFilesDepth, filename);

    Assert(scratchBufferFilesDepth, "File failed to load.")

    // Scan through the whole file and remove all line continuation
    //while (*fileStep2 != 0)
    //{
    //    if (*fileStep2 == '\\')
    //    {
    //        *fileStep2 = ' ';
    //        if (*(fileStep2 + 1) == '\n')
    //            *(fileStep2 + 1) = ' ';
    //        if (*(fileStep2 + 1) == '\r')
    //            *(fileStep2 + 1) = ' ';
    //        if (*(fileStep2 + 2) == '\n')
    //            *(fileStep2 + 2) = ' ';
    //        if (*(fileStep2 + 2) == '\r')
    //            *(fileStep2 + 2) = ' ';
    //    }
    //    fileStep2++;
    //}

    //while (fileStep3 != 0)
    //{
    //    char* next = GoToNextLineOrEndOfString(fileStep3);
    //    int len = next - fileStep3;
    //
    //    if (StartsWith(fileStep, "X("))
    //    {
    //
    //    }
    //
    //    fileStep3 = next;
    //}

    //int i = 0;
    while (fileStep != 0)
    {
        char* next = GoToNextLineOrEndOfString(fileStep);
        int len = next - fileStep;

        if (StartsWith(fileStep, "#include"))
        {
            char* a = fileStep + IndexOf(fileStep, '"') + 1;

            char name[100] = {};
            StringAppendMax(name, filename, LastIndexOf(filename, '/') + 1);
            int idx = StringLength(name) + IndexOf(a, '"');
            StringAppendMax(name, a, idx);
            if (EndsWith(name, ".shaderinc"))
            {
                destination = ShaderReadFile(shader, destination, scratchBufferFilesDepth, name, pixelShader, isPixelShader);
            }
        }
        else if (StartsWithIngoreWhitespace(fileStep, "#define Parameters"))
        {

        }
        else if (StartsWithIngoreWhitespace(fileStep, "X("))
        {
            char* type = fileStep + IndexOf(fileStep, '(')+1;
            char* name = type + IndexOf(type, ',')+1;
            char* end0 = name + IndexOf(name, ')');
            char* end1 = name + IndexOf(name, ',');
            char* end = end0 < end1 ? end0 : end1;


            destination = StringCopyLength(destination, "uniform ", 8);
            destination = StringCopyLength(destination, type, name - type-1);
            destination = StringCopyLength(destination, " ", 1);
            destination = StringCopyLength(destination, name, end - name);
            destination = StringCopyLength(destination, ";", 1);

            //destination = StringCopyLength(destination, fileStep, len);
        }
        else
        {
            destination = StringCopyLength(destination, fileStep, len);
        }

        if (*next == 0)
            break;
        fileStep = next;
    }
    return destination;
}

// Print the string peacemeal because android is trash.
void print_fuck_android(char* string)
{
    bool printing = true;
    int j = 0;
    char text[1000] = {};
    while (printing)
    {
        for (int i = 0; i < 999; i++)
        {
            text[i] = string[j];
            if(string[j] == 0)
            {
                printing = false;
                break;
            }
            j++;
        }
        text[999] = 0;
        haven->platformPrint(text);
    }
}

void LoadShader(Shader* shader)
{
    int maxSize = Kilobytes(100);

    char* scratchBuffer = (char*)(haven->scratchBuffer);

    char* pixelShader  = (char*)ArenaPushBytes(&haven->arenaAssets, maxSize, shader->filename, true);
    char* vertexShader = (char*)ArenaPushBytes(&haven->arenaAssets, maxSize, shader->filename, true);
    
    const char* pixelAppend = "#version 320 es\n#define pixelShader 1\n#define Cpp 0\n";
    const char* vertexAppend = "#version 320 es\n#define vertexShader 1\n#define Cpp 0\n";
    pixelShader = StringAppend(pixelShader, pixelAppend);
    vertexShader = StringAppend(vertexShader, vertexAppend);

    ShaderReadFile(shader, pixelShader  + StringLength(pixelAppend), scratchBuffer, shader->filename, pixelShader, true);
    ShaderReadFile(shader, vertexShader + StringLength(vertexAppend), scratchBuffer, shader->filename, vertexShader, false);
    
    int pixelShaderLength = StringLength(pixelShader);
    int vertexShaderLength = StringLength(vertexShader);

    //print_fuck_android(memory, vertexShader);

    Assert(pixelShaderLength < maxSize);
    Assert(vertexShaderLength < maxSize);

    shader->pixelShaderText = pixelShader;
    shader->vertexShaderText = vertexShader;
}
Shader* FileReadShader(const char* filename)
{
    //Shader* shader = &memory->shaders[ArrayCount(memory->shaders)];
    //ArrayCount(memory->shaders)++;
    //Assert(IsInArray(memory->shaders, ArrayCount(memory->shaders) - 1), "Array is out of capacity.");
    //memory->shaders[ArrayCount(memory->shaders) - 1] = {};

    ProfilerBeingSample();
    Shader* shader = ArrayAddNew(haven->shaders);

    StringCopy(shader->filename, filename);
    LoadShader(shader);
    ProfilerEndSample("LoadShader");
    return shader;
}

