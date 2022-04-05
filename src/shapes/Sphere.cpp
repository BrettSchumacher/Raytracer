#include "Sphere.h"

Sphere::Sphere()
{
    position = Vector3(0, 0, 0);
    materialIndex = 0;
    radius = 0;
}

Sphere::Sphere(Vector3 position, int matInd, Float radius)
{
    this->position = position;
    this->materialIndex = matInd;
    this->radius = radius;
}

bool Sphere::Intersect(Ray ray, RayHit& hitInfo)
{
    Vector3 offset = position - ray.origin;
    Float b = offset.dot(ray.direction); // no need to calculate a, as it is 1
    Float c = offset.dot(offset) - radius * radius;

    Float det2 = b * b - c;

    if (det2 < 0)
    {
        return false;
    }

    Float det = sqrt(det2);
    Float t = b - det; 
    bool inside = false;
    if (t < 0)
    {
        t = b + det;
        inside = true;
    }

    if (t < 0)
    {
        return false;
    }

    Vector3 hitPos = ray.origin + t * ray.direction;

    hitInfo.t = t;
    hitInfo.normal = (hitPos - position).normalized();
    hitInfo.position = hitPos;
    hitInfo.materialIndex = materialIndex;
    hitInfo.hit = true;
    hitInfo.inside = inside;
    SetUVInfo(hitInfo);

    return true;
}

WorldBounds Sphere::GetWorldBounds()
{
    Vector3 min = position - radius * Vector3::one;
    Vector3 max = position + radius * Vector3::one;
    return WorldBounds(min, max);
}

void Sphere::SetUVInfo(RayHit& hitInfo)
{
    Vector3 relPos = hitInfo.position - position;
    Vector3 norm = hitInfo.normal;

    Float phi = atan2(relPos.z, relPos.x);
    Float theta = acos(relPos.y / radius);

    hitInfo.uv.u = 1 - (phi + M_PI) / (2 * M_PI);
    hitInfo.uv.v = theta / M_PI;

    Vector3 tangent = Vector3(norm.z, 0, -norm.x);
    Vector3 bitangent = norm.cross(tangent);
    
    hitInfo.tangent = tangent.normalized();
    hitInfo.bitangent = bitangent.normalized();
}