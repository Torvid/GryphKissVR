#include "../haven.cpp"

#if structs
struct ReflectionProbe;

struct StaticMesh
{
    EntityContents;
    bool isSky;
    Material_defaultlit material;
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
    staticMesh->material = *material;
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
    staticMesh->material = {};
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

    if (t.scale.y < 0.01)
        t.scale.y = 0.01;
    if (t.scale.x < 0.01)
        t.scale.x = 0.01;
    if (t.scale.z < 0.01)
        t.scale.z = 0.01;

    return t;
}

RayHit StaticMeshLineTrace(StaticMesh* self, float3 start, float3 direction)
{
    Transform t = StaticMeshGetLocalBoundsTransform(self);
    float3 startLocal = WorldToLocal(start, t);
    float3 directionLocal = WorldToLocalVector(direction, t);
    RayHit hit = RayBoxIntersect(startLocal, directionLocal);
    hit.position = LocalToWorld(hit.position, t);
    if (hit.hit)
    {
        hit.distance = distance(hit.position, start);
        hit.entity = self;
    }
    return hit;
}
RayHit ReflectionProbeLineTrace(ReflectionProbe* self, float3 start, float3 direction)
{
    RayHit result = {};
    float dist = RaySphereIntersect(start - self->transform.position, direction, 0.4f);
    if (dist <= 0)
    {
        result.hit = false;
        return result;
    }

    result.hit = true;
    result.entity = self;
    result.distance = dist;
    result.position = start + direction * dist;
    return result;
}

RayHit StaticMeshLineTraceClosest(float3 start, float3 direction)
{
    RayHit closestHit = {};
    closestHit.distance = 99999999;
    for (int k = 0; k < ArrayCount(haven->entities); k++)
    {
        RayHit hit = {};
        if (haven->entities[k]->type == EntityType_StaticMesh)
        {
            hit = StaticMeshLineTrace((StaticMesh*)haven->entities[k], start, direction);
        }
        if (haven->entities[k]->type == EntityType_ReflectionProbe)
        {
            hit = ReflectionProbeLineTrace((ReflectionProbe*)haven->entities[k], start, direction);
        }
        if (hit.hit)
        {
            if (hit.distance < closestHit.distance)
            {
                closestHit = hit;
            }
        }
    }
    return closestHit;
}

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
        Rendering::DrawMesh(&self->material, self->mesh, self->transform, camera, "Scene StaticMesh");
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
        Rendering::DrawMesh(&self->material, self->mesh, self->transform, "Scene StaticMesh");
    }
}

void StaticMeshUpdate(StaticMesh* self, int i)
{
    Transform headLocal = input->head;
    if (StaticMeshCull(self, headLocal))
        return;
    
    Transform aabb = StaticMeshGetAABB(self);
    Transform bounds = StaticMeshGetLocalBoundsTransform(self);

    self->material.OverlayColor = self->OverlayColor;
    StaticMeshDraw(self);
}

#endif
