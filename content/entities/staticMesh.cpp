#pragma once
#include "../haven.cpp"

struct StaticMesh
{
    EntityContents;
    Material_defaultlit* material;
    Mesh* mesh;
};


StaticMesh* StaticMeshInstantiate(Mesh* mesh, Material_defaultlit* material, Transform transform)
{
    StaticMesh* staticMesh = Instantiate(StaticMesh);
    staticMesh->material = material;// gameState->barnWallCleanMat;
    staticMesh->mesh = mesh;// assets->BarnWall02;
    staticMesh->transform = transform;// transform(center + float3(0, x * 2, y), 0, 0, -0.25);
    return staticMesh;
}

Transform GetLocalBoundsTransform(StaticMesh* self)
{
    float3 boundsCenterWorldSpace = LocalToWorld(self->mesh->boundsCenter, self->transform);
    //float radius = length(self->transform.scale * self->mesh->boundsSize) * 0.5;
    //DrawAxisSphere(boundsCenterWorldSpace, radius, 0.01);

    Transform t = self->transform;
    t.position = boundsCenterWorldSpace;
    t.scale *= self->mesh->boundsSize * 0.5;

    if (t.scale.y == 0.0)
        t.scale.y = 0.01;
    if (t.scale.x == 0.0)
        t.scale.x = 0.01;
    if (t.scale.z == 0.0)
        t.scale.z = 0.01;

    return t;
}

bool CullMesh(StaticMesh* self, Transform camera)
{
    float3 boundsCenterWorldSpace = LocalToWorld(self->mesh->boundsCenter, self->transform);
    float radius = length(self->transform.scale * self->mesh->boundsSize) * 0.5;
    if (dot(camera.position - boundsCenterWorldSpace, camera.forward) > radius)
        return true;

    return false;
}

void StaticMeshUpdate(StaticMesh* self, int i)
{
    //if (CullMesh(self, input->head))
    //    return;

    //// frustum culling
    //float3 boundsCenterWorldSpace = LocalToWorld(self->mesh->boundsCenter, self->transform);
    //float radius = length(self->transform.scale * self->mesh->boundsSize) * 0.5;
    //if (dot(input->head.position - boundsCenterWorldSpace, input->head.forward) > radius)
    //    return;

    //if (i == 8)
    //{
    //    float3 boundsCenterWorldSpace = LocalToWorld(self->transform, self->mesh->boundsCenter);
    //    float radius = length(self->transform.scale * self->mesh->boundsSize) * 0.5;
    //    DrawAxisSphere(boundsCenterWorldSpace, radius, 0.01);
    //
    //    Transform t = GetLocalBoundsTransform(self);// self->transform;
    //    //t.position = boundsCenterWorldSpace;
    //    //t.scale *= self->mesh->boundsSize * 0.5;
    //    DrawBox(t);
    //    DrawTransform(t);
    //    DrawTransform(self->transform);
    //}

    DrawMesh(self->material, self->mesh, self->transform, "Scene StaticMesh");
}

