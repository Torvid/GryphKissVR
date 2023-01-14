#pragma once

//#define EntityTypeTable(n, X) \
//    X(n, StaticMesh) \
//    X(n, SkinnedMesh) \
//    X(n, Light) \
//    X(n, LightDirectional)
//MakeEnum(EntityType, EntityTypeTable);

//typedef void EntityStart(Entity* entity);
//typedef void EntityUpdate(Entity* entity);

    //EntityType type; \
//#define EntityUpdate(entity, name)

// forward-declare, defined in gryphkiss.cpp

#define EntityContents \
    bool alive; \
    Transform transform; \
    int type; \
    char* name[100]; \
    bool visible; \
    bool editorOnly;

struct Entity
{
    EntityContents;
};

//struct Light
//{
//    EntityContents;
//    float3 color;
//    float intensity;
//    float radius;
//};
//
//struct StaticMesh
//{
//    EntityContents;
//    Mesh* mesh;
//    Material* material;
//};
//
//struct SkinnedMesh
//{
//    EntityContents;
//    Mesh* mesh;
//    Material* material;
//    Animation* animation;
//    float animationTime;
//};
//
//struct LightDirectional
//{
//    EntityContents;
//    float3 color;
//    float intensity;
//    float radius;
//};

#define Instantiate(_type) (_type*)InstantiateLocal(EntityType_##_type, sizeof(_type));
void* InstantiateLocal(int type, uint32 size)
{
    Entity* newEntity = (Entity*)ArenaPushBytes(&haven->arenaScene, size, "ent");// (haven->arenaScene.base + haven->arenaScene.used);
    newEntity->transform = transform(float3(0, 0, 0));
    newEntity->type = type;
    ArrayAdd(haven->entities, newEntity);
    return (void*)newEntity;
}

