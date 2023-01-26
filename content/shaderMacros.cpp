
#define MakeStructVariable(type, name, ...) type name;
#define MakeStructPointer(type, name, ...) uint8* name##Ptr;

#define MakeEnumEntry(type, name, ...) PASTE(PASTE(Enum_, ShaderName), _##name),
#define MakeStringEntry(type, name, ...) #name,
#define MakeTypeEntry(type, name, ...) type,

#define _PASTE(x, y) x ## y
#define PASTE(x, y) _PASTE(x, y)


#define SetStructPointer(type, name, ...) temp->name##Ptr = (uint8*)&temp->name;

#define ShaderStructName PASTE(Material_, ShaderName)

#define EnumName PASTE(Enum_, ShaderName)
#define EnumCount PASTE(EnumName, _COUNT) + 1
#define EnumCount2 PASTE(EnumName, _COUNT)

#define TypesName PASTE(Types_, ShaderName)
#define StringsName PASTE(Strings_, ShaderName)

enum EnumName
{
    Parameters(MakeEnumEntry) PASTE(EnumName, _COUNT)
};
char* StringsName[EnumCount] = \
{
    Parameters(MakeStringEntry) ""
};
#define float 0
#define float2 1
#define float3 2
#define float4 3
#undef sampler2D
#define sampler2D 4
int TypesName[EnumCount] = \
{
    Parameters(MakeTypeEntry) - 1
};
#undef float
#undef float2
#undef float3
#undef float4
#undef sampler2D
#undef float4x4

#define float2(x, y) ctor_float2((x), (y))
#define float3(x, y, z) ctor_float3((x), (y), (z))

#define sampler2D Texture*

struct ShaderStructName
{
    union
    {
        Material mat;
        struct
        {
            MaterialContents
        };
    };

    Parameters(MakeStructVariable)
#if addBoneTransforms
        Transform shaderBoneTransforms[200];
#endif
    union
    {
        uint8* shaderParameters[EnumCount2];
        struct
        {
            Parameters(MakeStructPointer)
        };
    };
};

void DrawMesh(ShaderStructName* command, const char* name = 0)
{
    ShaderStructName* temp;
    //if (text)
    //    temp = (ShaderStructName*)ArenaPushStruct(&haven->arenaDrawTextCommands, ShaderStructName, command->mesh->filename);
    //else
    temp = (ShaderStructName*)ArenaPushStruct(&haven->arenaDrawCommands, ShaderStructName, command->mesh->filename);
    *temp = *command;

    Parameters(SetStructPointer);
    temp->parameters = &temp->shaderParameters[0];

    RenderCommand* result = ArrayAddNew(haven->renderCommands);
    result->index = ArrayCount(haven->renderCommands);
    result->type = RenderCommand_DrawMesh;
    result->material = &temp->mat;
}

void DrawMesh(ShaderStructName* command, Mesh* mesh, Transform transform, 
    Camera camera, const char* name = 0)
{
    ShaderStructName* temp;
    //if (text)
    //    temp = (ShaderStructName*)ArenaPushStruct(&haven->arenaDrawTextCommands, ShaderStructName, mesh->filename);
    //else
    temp = (ShaderStructName*)ArenaPushStruct(&haven->arenaDrawCommands, ShaderStructName, mesh->filename);
    *temp = *command;

    Parameters(SetStructPointer)
    temp->parameters = &temp->shaderParameters[0];

    temp->mesh = mesh;
    if (mesh->skinned)
    {
        temp->boneTransforms = &temp->shaderBoneTransforms[0];
    }
    temp->transform = transform;
    RenderCommand* result = ArrayAddNew(haven->renderCommands);
    result->index = ArrayCount(haven->renderCommands);
    result->type = RenderCommand_DrawMesh;
    result->material = &temp->mat;
    result->name = name;
    result->customCamera = camera;

}
void DrawMesh(ShaderStructName* command, Mesh* mesh, Transform transform, const char* name = 0)
{
    DrawMesh(command, mesh, transform, {}, name);
}

void PASTE(SetupShader_, ShaderName)(Shader* shader)
{
    shader->parameterCount = EnumCount2;
    shader->parameterNames = StringsName;
    shader->parameterTypes = TypesName;
}

// variable arguments macro
#define GET_MACRO2(_0, _1, _2, _3, NAME, ...) NAME
#define MakeSetDefault(...) GET_MACRO2(_0, ##__VA_ARGS__, \
        MakeSetDefault3, \
        MakeSetDefault2, \
        MakeSetDefault1, \
        MakeSetDefault0 )(__VA_ARGS__)
#define MakeSetDefault0()
#define MakeSetDefault1(arg0)
#define MakeSetDefault2(arg0, arg1)
#define MakeSetDefault3(type, name, def) command->name = def;

void SetShaderDefaults(ShaderStructName* command)
{
    Parameters(MakeSetDefault)
}
