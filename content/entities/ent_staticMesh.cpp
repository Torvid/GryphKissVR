#include "../haven.cpp"

#if structs

struct StaticMesh
{
    EntityContents;
    bool isSky;
    Material_defaultlit* material;
    Material_skydomeShader* materialSky;
    Mesh* mesh;
};


// forward-declare
void StaticMeshDraw(StaticMesh* self, Camera camera);
bool StaticMeshCull(StaticMesh* self, Transform camera);
#else

StaticMesh* StaticMeshInstantiate(Mesh* mesh, Material_defaultlit* material, Transform transform)
{
    Assert(mesh, "mesh was null.");
    Assert(material, "material was null.");
    StaticMesh* staticMesh = Instantiate(StaticMesh);
    staticMesh->isSky = false;
    staticMesh->material = material;
    staticMesh->materialSky = 0;
    staticMesh->mesh = mesh;
    staticMesh->transform = transform;
    return staticMesh;
}
StaticMesh* StaticMeshInstantiate(Mesh* mesh, Material_skydomeShader* material, Transform transform)
{
    Assert(mesh, "mesh was null.");
    Assert(material, "material was null.");
    StaticMesh* staticMesh = Instantiate(StaticMesh);
    staticMesh->isSky = true;
    staticMesh->material = 0;
    staticMesh->materialSky = material;
    staticMesh->mesh = mesh;
    staticMesh->transform = transform;
    return staticMesh;
}

Transform StaticMeshGetLocalBoundsTransform(StaticMesh* self)
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

bool StaticMeshCull(StaticMesh* self, Transform camera)
{
    float3 boundsCenterWorldSpace = LocalToWorld(self->mesh->boundsCenter, self->transform);
    float radius = length(self->transform.scale * self->mesh->boundsSize) * 0.5;
    if (dot(camera.position - boundsCenterWorldSpace, camera.forward) > radius)
        return true;

    return false;
}

void StaticMeshDraw(StaticMesh* self, Camera camera)
{
    if(self->isSky)
        Rendering::DrawMesh(self->materialSky, self->mesh, self->transform, camera, "Scene StaticMesh");
    else
        Rendering::DrawMesh(self->material, self->mesh, self->transform, camera, "Scene StaticMesh");
}
void StaticMeshDraw(StaticMesh* self)
{
    if (self->isSky)
        Rendering::DrawMesh(self->materialSky, self->mesh, self->transform, "Scene StaticMesh");
    else
        Rendering::DrawMesh(self->material, self->mesh, self->transform, "Scene StaticMesh");
}

void StaticMeshUpdate(StaticMesh* self, int i)
{
    Transform headLocal = input->head;// LocalToWorld(input->head, input->playspace);
    if (StaticMeshCull(self, headLocal))
        return;

    //// frustum culling
    //float3 boundsCenterWorldSpace = LocalToWorld(self->mesh->boundsCenter, self->transform);
    //float radius = length(self->transform.scale * self->mesh->boundsSize) * 0.5;
    //if (dot(input->head.position - boundsCenterWorldSpace, input->head.forward) > 0)
    //    return;
    //
    //if (i == 8)
    //{
    //    float3 boundsCenterWorldSpace = LocalToWorld(self->mesh->boundsCenter, self->transform);
    //    float radius = length(self->transform.scale * self->mesh->boundsSize) * 0.5;
    //    DrawAxisSphere(boundsCenterWorldSpace, radius, 0.01);
    //
    //    Transform t = GetLocalBoundsTransform(self);// self->transform;
    //    //t.position = boundsCenterWorldSpace;
    //    //t.scale *= self->mesh->boundsSize * 0.5;
    //    DrawBox(t);
    //    DrawTransform(t);
    //    DrawTransform(self->transform);
    //
    //    DrawSphere(input->head.position, 0.5, 0.01, float3(1, 0, 0));
    //}
    
    StaticMeshDraw(self);
    //if(self->isSky)
    //    DrawMesh(self->materialSky, self->mesh, self->transform, "Scene StaticMesh");
    //else
    //    DrawMesh(self->material, self->mesh, self->transform, "Scene StaticMesh");
}

#endif