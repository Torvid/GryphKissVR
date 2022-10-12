#pragma once

char* ShaderReadFile(EngineState* engineState, Shader* shader, char* destination, char* scratchBufferFilesDepth, char* filename, char* pixelShader, bool isPixelShader)
{
    char* fileStep = scratchBufferFilesDepth;
    char* fileStep2 = scratchBufferFilesDepth;
    char* fileStep3 = scratchBufferFilesDepth;
    scratchBufferFilesDepth = (char*)engineState->platformReadFile((uint8*)scratchBufferFilesDepth, filename);

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
            if (EndsWith(name, ".shader"))
            {
                destination = ShaderReadFile(engineState, shader, destination, scratchBufferFilesDepth, name, pixelShader, isPixelShader);
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
void print_fuck_android(EngineState* memory, char* string)
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
        memory->platformPrint(text);
    }
}

void LoadShader(EngineState* memory, Shader* shader)
{
    char* scratchBuffer = (char*)(memory->scratchBuffer);

    char* pixelShader  = (char*)ArenaPushBytes(&memory->arenaHotreload, Kilobytes(25), shader->filename, true);
    char* vertexShader = (char*)ArenaPushBytes(&memory->arenaHotreload, Kilobytes(25), shader->filename, true);
    
    const char* pixelAppend = "#version 320 es\n#define pixelShader 1\n#define Cpp 0\n";
    const char* vertexAppend = "#version 320 es\n#define vertexShader 1\n#define Cpp 0\n";
    pixelShader = StringAppend(pixelShader, pixelAppend);
    vertexShader = StringAppend(vertexShader, vertexAppend);



    ShaderReadFile(memory, shader, pixelShader  + StringLength(pixelAppend), scratchBuffer, shader->filename, pixelShader, true);
    ShaderReadFile(memory, shader, vertexShader + StringLength(vertexAppend), scratchBuffer, shader->filename, vertexShader, false);
    
    int pixelShaderLength = StringLength(pixelShader);
    int vertexShaderLength = StringLength(vertexShader);

    //print_fuck_android(memory, vertexShader);

    Assert(pixelShaderLength < Kilobytes(25));
    Assert(vertexShaderLength < Kilobytes(25));

    shader->pixelShaderText = pixelShader;
    shader->vertexShaderText = vertexShader;
}
Shader* FileReadShader(EngineState* memory, const char* filename)
{
    //Shader* shader = &memory->shaders[ArrayCount(memory->shaders)];
    //ArrayCount(memory->shaders)++;
    //Assert(IsInArray(memory->shaders, ArrayCount(memory->shaders) - 1), "Array is out of capacity.");
    //memory->shaders[ArrayCount(memory->shaders) - 1] = {};

    Shader* shader = ArrayAddNew(memory->shaders);

    StringCopy(shader->filename, filename);
    LoadShader(memory, shader);
    return shader;
}

