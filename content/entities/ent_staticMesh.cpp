#include "../haven.cpp"

#if structs
struct ReflectionProbe;

struct StaticMesh
{
    EntityContents;
    bool isSky;
    Material_defaultlit* material;
    Material_skydomeShader* materialSky;
    Mesh* mesh;
    ReflectionProbe* reflectionProbe;
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
    float3 boundsCenterWorldSpace = LocalToWorld(self->mesh->boundsTransform.position, self->transform);
    //float radius = length(self->transform.scale * self->mesh->boundsSize) * 0.5;
    //DrawAxisSphere(boundsCenterWorldSpace, radius, 0.01);

    Transform t = self->transform;
    t.position = boundsCenterWorldSpace;
    t.scale *= self->mesh->boundsTransform.scale;

    if (t.scale.y == 0.0)
        t.scale.y = 0.01;
    if (t.scale.x == 0.0)
        t.scale.x = 0.01;
    if (t.scale.z == 0.0)
        t.scale.z = 0.01;

    return t;
}
bool RayWorldIntersect(float3 pos0, float3 pos1)
{
    for (int k = 0; k < ArrayCount(haven->entities); k++)
    {
        if (haven->entities[k]->type == EntityType_StaticMesh)
        {
            StaticMesh* mesh = (StaticMesh*)haven->entities[k];
            Transform t = StaticMeshGetLocalBoundsTransform(mesh);
            float3 startLocal = WorldToLocal(pos0, t);
            float3 endLocal = WorldToLocal(pos1, t);
            if (RayBoxIntersect(startLocal, endLocal))
            {
                return true;
            }
        }
    }
    return false;
}

//bool BoundsOverlap(StaticMesh* mesh, ReflectionProbe* probe)
//{
//
//}


Transform StaticMeshGetAABB(StaticMesh* self)
{
    Transform bounds = StaticMeshGetLocalBoundsTransform(self);
    float3 pos0 = bounds.position +
        bounds.right     * bounds.scale.x +
        bounds.forward   * bounds.scale.y +
        bounds.up        * bounds.scale.z;
    float3 pos1 = bounds.position +
        -bounds.right    * bounds.scale.x +
        bounds.forward   * bounds.scale.y +
        bounds.up        * bounds.scale.z;
    float3 pos2 = bounds.position +
        bounds.right     * bounds.scale.x +
        -bounds.forward  * bounds.scale.y +
        bounds.up        * bounds.scale.z;
    float3 pos3 = bounds.position +
        -bounds.right    * bounds.scale.x +
        -bounds.forward  * bounds.scale.y +
        bounds.up        * bounds.scale.z;
    float3 pos4 = bounds.position +
        bounds.right     * bounds.scale.x +
        bounds.forward   * bounds.scale.y +
        -bounds.up       * bounds.scale.z;
    float3 pos5 = bounds.position +
        -bounds.right    * bounds.scale.x +
        bounds.forward   * bounds.scale.y +
        -bounds.up       * bounds.scale.z;
    float3 pos6 = bounds.position +
        bounds.right     * bounds.scale.x +
        -bounds.forward  * bounds.scale.y +
        -bounds.up       * bounds.scale.z;
    float3 pos7 = bounds.position +
        -bounds.right    * bounds.scale.x +
        -bounds.forward  * bounds.scale.y +
        -bounds.up       * bounds.scale.z;

    float3 boundsMin = min(min(min(min(min(min(min(pos0, pos1), pos2), pos3), pos4), pos5), pos6), pos7);
    float3 boundsMax = max(max(max(max(max(max(max(pos0, pos1), pos2), pos3), pos4), pos5), pos6), pos7);
    float3 boundsCenter = (boundsMin + boundsMax) * 0.5f;
    float3 boundsSize = abs(boundsMin - boundsMax) * 0.5f;

    Transform aabb = transform();
    aabb.position = boundsCenter;
    aabb.scale = boundsSize;

    return aabb;
}

bool StaticMeshCull(StaticMesh* self, Transform camera)
{
    float3 boundsCenterWorldSpace = LocalToWorld(self->mesh->boundsTransform.position, self->transform);
    //float radius = length(self->transform.scale * self->mesh->boundsSize) * 0.5;
    if (dot(camera.position - boundsCenterWorldSpace, camera.forward) > self->mesh->radius)
        return true;

    return false;
}

void StaticMeshDraw(StaticMesh* self, Camera camera)
{
    if (self->isSky)
    {
        Rendering::DrawMesh(self->materialSky, self->mesh, self->transform, camera, "Scene StaticMesh");
    }
    else
    {
        //self->material->texCubemap0 = self->cubemap;
        Rendering::DrawMesh(self->material, self->mesh, self->transform, camera, "Scene StaticMesh");
    }
}
void StaticMeshDraw(StaticMesh* self)
{
    if (self->isSky)
    {
        Rendering::DrawMesh(self->materialSky, self->mesh, self->transform, "Scene StaticMesh");
    }
    else
    {
        Rendering::DrawMesh(self->material, self->mesh, self->transform, "Scene StaticMesh");
    }
}

void StaticMeshUpdate(StaticMesh* self, int i)
{
    Transform headLocal = input->head;
    if (StaticMeshCull(self, headLocal))
        return;
    
    //Transform aabb = StaticMeshGetAABB(self);

    Transform aabb = StaticMeshGetAABB(self);
    Transform bounds = StaticMeshGetLocalBoundsTransform(self);
    Drawing::DrawBox(aabb, 0.01);

    StaticMeshDraw(self);
}

#endif
