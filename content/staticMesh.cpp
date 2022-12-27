#pragma once
#include "haven.cpp"

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

void StaticMeshUpdate(StaticMesh* staticMesh, int i)
{
    float radius = length(staticMesh->transform.scale * staticMesh->mesh->boundsSize) * 0.5;
    
    float3 boundsCenterWorldSpace = LocalToWorld(staticMesh->transform, staticMesh->mesh->boundsCenter);

    // frustum culling
    //if (dot(input->head.position - boundsCenterWorldSpace, input->head.forward) > radius)
    //    return;

    if (i == 3)
    {
        DrawAxisSphere(boundsCenterWorldSpace, radius, 0.01);

        Transform t = staticMesh->transform;
        t.position = boundsCenterWorldSpace;
        t.scale *= staticMesh->mesh->boundsSize * 0.5;
        DrawBox(t);
        DrawTransform(t);
        DrawTransform(staticMesh->transform);
    }

    DrawMesh(staticMesh->material, staticMesh->mesh, staticMesh->transform);
}

