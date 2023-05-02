#pragma once

#define GET_MACRO(_0, _1, _2, NAME, ...) NAME
#define Assert(...) GET_MACRO(_0, ##__VA_ARGS__, Assert2, Assert1, Assert0)(__VA_ARGS__)
#define Assert1(expression)      if(!(expression)) { *(int*)0 = 0; }
#define Assert2(expression, msg) if(!(expression)) { const char* message = msg; *(int*)0 = 0; }

//#define Assert(expression)         if(!(expression)) { *(int*)0 = 0; }
//#define AssertMsg(expression, msg) if(!(expression)) { const char* message = msg; *(int*)0 = 0; }

#define ArrayCapacity(Array) (sizeof(Array) / sizeof((Array)[0]))


typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

#define Kilobytes(x) ((x) * (1024LL))
#define Megabytes(x) ((Kilobytes(x)) * (1024LL))
#define Gigabytes(x) ((Megabytes(x)) * (1024LL))
#define Terabytes(x) ((Gigabytes(x)) * (1024LL))


typedef struct 
{
    float x;
    float y;
} float2;
typedef struct 
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };
        struct
        {
            float2 xy;
        };
    };
} float3;
typedef struct 
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
        struct
        {
            float2 xy;
            float2 zw;
        };
        struct
        {
            float3 xyz;
        };
    };
} float4;
typedef struct
{
    uint8 x;
    uint8 y;
    uint8 z;
    uint8 w;
} byte4;
typedef struct 
{
    int x;
    int y;
} int2;
//struct int3
//{
//    int x;
//    int y;
//    int z;
//};
//struct int4
//{
//    int x;
//    int y;
//    int z;
//    int w;
//};

#define TransformContents \
    float3 position; \
    float3 right; \
    float3 forward; \
    float3 up; \
    float3 scale;

typedef struct
{
    TransformContents
} Transform;

typedef struct
{
    float4 m1;
    float4 m2;
    float4 m3;
    float4 m4;
} float4x4;

typedef struct 
{
    bool Held; // Button is being held down
    bool Down; // Button changed from down to up
    bool Up; // Button changed from up to down
    bool LastHeld;
    bool Temp;
    bool Changed;
} ButtonState;
typedef struct 
{
    int32 Value;
    int32 LastValue;
    int32 ValueDelta;
} IntState;
typedef struct 
{
    float Value;
    float LastValue;
    float ValueDelta;
} FloatState;
typedef struct 
{
    float2 Value;
    float2 LastValue;
    float2 ValueDelta;
} Float2State;
typedef struct 
{
    float3 Value;
    float3 LastValue;
    float3 ValueDelta;
} Float3State;

// look at the cringe code that header files makes me have to write.
#define InputUpdateButton(Button, asdf123) \
{\
    (Button)->Held = (asdf123);\
    (Button)->Up = false;\
    (Button)->Down = false;\
    if ((Button)->Held != (Button)->LastHeld)\
    {\
        (Button)->LastHeld = (Button)->Held;\
        if ((Button)->Held)\
        {\
            (Button)->Up = true;\
        }\
        else\
        {\
            (Button)->Down = true;\
        }\
    }\
}\

#define InputUpdateFloat2(Button, asdf123)\
{\
    (Button)->LastValue.x = (Button)->Value.x;\
    (Button)->LastValue.y = (Button)->Value.y;\
    (Button)->Value.x = asdf123.x;\
    (Button)->Value.y = asdf123.y;\
    (Button)->ValueDelta.x = (Button)->Value.x - (Button)->LastValue.x;\
    (Button)->ValueDelta.y = (Button)->Value.y - (Button)->LastValue.y;\
}\

#define InputUpdateFloat(Button, asdf123)\
{\
    (Button)->Value = asdf123;\
    (Button)->ValueDelta = (Button)->Value - (Button)->LastValue;\
    (Button)->LastValue = (Button)->Value;\
}\

#define UNION_BUTTON(name) union { ButtonState State##name; struct { bool name; bool name##Up; bool name##Down; }; }
#define UNION_INT(name) union { IntState State##name; struct { int32 name; int32 Last##name; int32 name##Delta; }; }
#define UNION_FLOAT(name) union { FloatState State##name; struct { float name; float Last##name; float name##Delta; }; }
#define UNION_FLOAT2(name) union { Float2State State##name; struct { float2 name; float2 Last##name; float2 name##Delta; }; }
#define UNION_FLOAT3(name) union { Float3State State##name; struct { float3 name; float3 Last##name; float3 name##Delta; }; }

typedef struct 
{
    union
    {
        Transform playspaces[7];
        struct
        {
            Transform playspaceViewFront;
            Transform playspaceLocal;
            Transform playspaceStage;
            Transform playspaceStageLeft;
            Transform playspaceStageRight;
            Transform playspaceStageLeftRotated;
            Transform playspaceStageRightRotated;
        };
    };

    Transform playspace;


    Transform head;

    Transform eyeLeft;
    Transform handLeft;
    Transform aimLeft;
    UNION_BUTTON(faceButtonLeft);
    UNION_FLOAT2(thumbstickLeft);
    UNION_FLOAT(triggerLeft);
    UNION_FLOAT(grabLeft);
    float vibrationAmplitudeLeft;

    Transform eyeRight;
    Transform handRight;
    Transform aimRight;
    UNION_BUTTON(faceButtonRight);
    UNION_FLOAT2(thumbstickRight);
    UNION_FLOAT(triggerRight);
    UNION_FLOAT(grabRight);
    float vibrationAmplitudeRight;


    UNION_FLOAT2(mousePos);
    UNION_INT(mouseScroll);

    UNION_BUTTON(mouseLeft);
    UNION_BUTTON(mouseRight);
    UNION_BUTTON(mouseMiddle);

    union
    {
        ButtonState letters[26];
        struct
        {
            UNION_BUTTON(a);
            UNION_BUTTON(b);
            UNION_BUTTON(c);
            UNION_BUTTON(d);
            UNION_BUTTON(e);
            UNION_BUTTON(f);
            UNION_BUTTON(g);
            UNION_BUTTON(h);
            UNION_BUTTON(i);
            UNION_BUTTON(j);
            UNION_BUTTON(k);
            UNION_BUTTON(l);
            UNION_BUTTON(m);
            UNION_BUTTON(n);
            UNION_BUTTON(o);
            UNION_BUTTON(p);
            UNION_BUTTON(q);
            UNION_BUTTON(r);
            UNION_BUTTON(s);
            UNION_BUTTON(t);
            UNION_BUTTON(u);
            UNION_BUTTON(v);
            UNION_BUTTON(w);
            UNION_BUTTON(x);
            UNION_BUTTON(y);
            UNION_BUTTON(z);
        };
    };
    
    union
    {
        ButtonState numbers[10];
        struct
        {
            UNION_BUTTON(d0);
            UNION_BUTTON(d1);
            UNION_BUTTON(d2);
            UNION_BUTTON(d3);
            UNION_BUTTON(d4);
            UNION_BUTTON(d5);
            UNION_BUTTON(d6);
            UNION_BUTTON(d7);
            UNION_BUTTON(d8);
            UNION_BUTTON(d9);
        };
    };

    UNION_BUTTON(space);
    UNION_BUTTON(backspace);
    UNION_BUTTON(enter);
    UNION_BUTTON(escape);
    UNION_BUTTON(del);
    UNION_BUTTON(arrowLeft);
    UNION_BUTTON(arrowRight);
    UNION_BUTTON(arrowUp);
    UNION_BUTTON(arrowDown);
    UNION_BUTTON(shift);
    UNION_BUTTON(ctrl);
    UNION_BUTTON(alt);

    float deltaTime;
    float time;

} Input;

typedef struct 
{
    int16 left;
    int16 right;
} Sample;


typedef struct 
{
    float3 position;
    float3 normal;
    float3 tangent;
    float2 uv0;
    float2 uv1;
    int materialID;
} Vertex;

typedef struct
{
    float3 position;
    float3 normal;
    float3 tangent;
    float2 uv0;
    float2 uv1;
    int materialID;
    float4 boneWeights;
    byte4 boneIndexes;
} VertexSkinned;

//struct Bone;
typedef struct Bone Bone;

struct Bone
{
    int index;
    Bone* parent;

    // 50 ptrs? this is fine?
    int children_count;
    Bone* children[50];
    char* name; // ptr to 100 char string
};

typedef struct 
{
    char filename[100];
    int GLID; // OpenGL ID
    uint32 m_cubeVertexBuffer;
    uint32 m_cubeIndexBuffer;

    uint32 indexCount;
    uint32 vertexCount;

    uint32* indexes;

    Vertex* vertexes;
    uint32 skinned; // bool
    VertexSkinned* skinnedVertexes;

    int boneCount;
    Bone boneHierarchy[200];
    Transform* bindPose;

    float3 boundsCenter;
    float3 boundsSize;
    float3 boundsMin;
    float3 boundsMax;
    float radius;
} Mesh;

typedef struct
{
    char filename[100];
    uint32 boneCount;
    uint32 frameCount;
    Transform* transforms;
    float length;
} Animation;

// sample size is always int16
typedef struct 
{
    char filename[100];
    bool loaded;
    int sampleCount;
    float length;
    Sample* data;
} Sound;

typedef struct 
{
    char filename[100];
    int GLID; // OpenGL ID
    int sizeX;
    int sizeY;
    float2 size;
    float aspectRatio;

    int mipCount;
    uint8* mips[13];
    int mipSize[13];
    int mipSizeX[13];
    int mipSizeY[13];

    bool ASTC; // compressed
    bool HDR; // float32

    bool isFramebufferTarget;
    int GLID_left; // left eye
    int GLID_right; // right eye
    int GLID_spectator; // spectator view

    bool isTextureTarget;
} Texture;

typedef struct 
{
    char filename[100];
    int GLID;
    int m_modelViewProjectionUniformLocation;
    int m_modelUniformLocation;
    
    int m_resolutionUniformLocation;
    int m_isSpectatorViewUniformLocation;
    int m_isSkinnedUniformLocation;
    int m_boneTransformsUniformLocation;
    int m_CameraPositionUniformLocation;
    int m_CameraForwardUniformLocation;

    int m_vertexAttribCoords;
    int m_vertexAttribNormal;
    int m_vertexAttribTangent;
    int m_vertexAttribUV0;
    int m_vertexAttribUV1;
    int m_materialID;
    int m_vertexAttribBoneIndexes;
    int m_vertexAttribBoneWeights;

    char* pixelShaderText;
    char* vertexShaderText;

    int parameterCount;
    char parameterNames[100][100]; // ~8k
    int parameterTypes[100]; // 
    int GLLocations[100];

} Shader;

typedef enum
{
    BlendMode_Opaque,
    BlendMode_Alpha,
    BlendMode_Add,
} BlendMode;

#define MaterialContents \
    Transform transform; \
    Mesh* mesh; \
    Shader* shader; \
    bool BackFaceCulling; \
    bool Wireframe; \
    bool DisableDepthTest; \
    BlendMode blendMode; \
    bool spectatorOnly; \
    uint8** parameters;\
    Transform* boneTransforms;

typedef struct 
{
    MaterialContents

} Material;

typedef enum
{
    RenderCommand_Invalid,
    RenderCommand_DrawMesh,
    RenderCommand_Clear,
    RenderCommand_SetFramebufferTarget,
    RenderCommand_SetRenderTarget,
    RenderCommand_Max
} RenderCommandType;

typedef struct
{
    bool enabled;
    Transform transform;
    float fov;
    bool orthographic;
    float orthoWidth;
    float maxDepth;
    float aspectRatio;
} Camera;

//#define RenderCommandCommon \

typedef struct 
{
    RenderCommandType type;
    const char* name;
    int index;

    // RenderCommand_DrawMesh
    Material* material;
    bool customCameraEnabled;
    Camera customCamera;

    // RenderCommand_Clear
    float4 clearColor;
    bool clearColorEnabled;
    bool clearDepthEnabled;

    // RenderCommand_SetFramebufferTarget
    Texture* framebufferTarget;

    // RenderCommand_SetRenderTarget
    Texture* textureTarget;
    
} RenderCommand;

// Reads a file into memory.
typedef uint8* PlatformReadFile(uint8* destination, const char* filename);
typedef bool PlatformWriteFile(uint8* source, int length, const char* inFilename);
typedef int64 PlatformTime();
typedef void PlatformPrint(const char* text);
typedef int Printf(char const* const _Format, ...);
typedef int sPrintf(char* str, const char* format, ...);
typedef int GLUploadTexture(Texture* texture);
typedef int GLUploadMesh(Mesh* texture);
typedef void* Memset(void* str, int c, uint64 n);
typedef void* Memcpy(void* destination, const void* source, uint64 num);

typedef uint32 PlatformGraphicsLoadTexture(Texture* texture, bool clamp, bool generateMipmaps); // Upload a texture to the GPU and return back a handle
typedef uint32 PlatformGraphicsLoadMesh(Mesh* mesh); // Upload a mesh to the GPU and return back a handle
typedef uint32 PlatformGraphicsLoadShader(Shader* shader); // Compile and upload a unlit to the GPU and return back a handle
typedef void PlatformGraphicsCreateFramebufferTarget(Texture* texture); // Create a framebuffer, these are special because they contain up to three textures. (left eye, right eye, spectator view)
typedef void PlatformGraphicsCreateTextureTarget(Texture* texture, bool clamp); // Create a texture target for drawing to. use for GPU sim and such.

typedef void PlatformGraphicsReadbackTexture(Texture* texture, void* pixels); // Create a texture target for drawing to. use for GPU sim and such.
typedef void PlatformGraphicsReadbackTextureHDR(Texture* texture, void* pixels); // Create a texture target for drawing to. use for GPU sim and such.

typedef void EngineProfilerBeingSample();
typedef void EngineProfilerEndSample(char* name);


typedef struct 
{
    bool initialized;
    bool gameFocused;
    int lastFrame;
    int frame;
    PlatformPrint* platformPrint;
    Printf* printf;
    sPrintf* sprintf;
    Memset* memset;
    Memcpy* memcpy;
    PlatformTime* platformTime;

    PlatformReadFile* platformReadFile;
    PlatformWriteFile* platformWriteFile;

    PlatformGraphicsLoadTexture* platformGraphicsLoadTexture;
    PlatformGraphicsLoadMesh* platformGraphicsLoadMesh;
    PlatformGraphicsLoadShader* platformGraphicsLoadShader;
    PlatformGraphicsCreateFramebufferTarget* platformGraphicsCreateFramebufferTarget;
    PlatformGraphicsCreateTextureTarget* platformGraphicsCreateTextureTarget;
    PlatformGraphicsReadbackTexture* platformGraphicsReadbackTexture;
    PlatformGraphicsReadbackTextureHDR* platformGraphicsReadbackTextureHDR;

    EngineProfilerBeingSample* engineProfilerBeingSample;
    EngineProfilerEndSample* engineProfilerEndSample;

    Transform spectatorCamera;
    float spectatorCameraMatrix[16];

    bool reloadNow; // trigger the game to reaload everything

    // These are loaded and set by the game on init. Used as fallbacks.
    Texture* missingTexture;
    Mesh* missingMesh;
    Shader* missingShader;

    //int32 SamplesPerSecond;
    int32 SampleCount;
    Sample Samples[44100];


    Input input;
    
    // Resolution of the monitor
    float2 Resolution;

    // Rendering/
    int renderCommands_count;
    RenderCommand* renderCommands;// [5000] ;

    float* GlobalVariables;

    bool first;
    //int currentShader;
    //int currentMesh;
    bool currentBackFaceCulling;
    BlendMode currentBlendMode;
    bool currentWireframe;
    bool currentDisableDepthTest;
    bool currentSkinned;
    Shader* currentShader;

    uint8 memory;
} GameMemory;

typedef void UpdateAndRenderTemplate(GameMemory* memory);


typedef struct 
{
    float3 positions[200000];
    float3 normals[200000];
    float2 uvs[200000];
    uint32 indexes[200000 * 3];
} ObjMeshTemp;
