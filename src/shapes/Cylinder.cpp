#include "Cylinder.h"

Cylinder::Cylinder()
{
    position = Vector3(0.0f, 0.0f, 0.0f);
    materialIndex = 0;
    radius = 0.0f;
    height = 0.0f;
    localY = Vector3(0.0f, 1.0f, 0.0f);
    CalculateLocalBasis();
}

Cylinder::Cylinder(Vector3 position, Vector3 upDir, int matInd, Float radius, Float height)
{
    this->position = position;
    this->materialIndex = matInd;
    this->radius = radius;
    this->height = height;
    this->localY = upDir.normalized();
    CalculateLocalBasis();
}

void Cylinder::SetUpDir(Vector3 upDir)
{
    this->localY = upDir.normalized();
    CalculateLocalBasis();
}

Vector3 Cylinder::GetUpDir()
{
    return localY;
}

void Cylinder::CalculateLocalBasis()
{
    if (localY.y != 0.0f)
    {
        localX = localY.cross(Vector3::forward).normalized();
    }
    else
    {
        localX = localY.cross(Vector3::up).normalized();
    }

    localZ = localX.cross(localY).normalized();

    // since we're using orthonomal basis, we can get inverses by transposing the 3x3 matrix of [localX, localY, localZ]
    localXInv.x = localX.x; localXInv.y = localY.x; localXInv.z = localZ.x;
    localYInv.x = localX.y; localYInv.y = localY.y; localYInv.z = localZ.y;
    localZInv.x = localX.z; localZInv.y = localY.z; localZInv.z = localZ.z;
}

// Cylinder's intersect function is kind of a mess... there are a lot of cases based on where the ray origin is, 
// inside the cylinder, outside the cylinder, above/below the caps...
bool Cylinder::Intersect(Ray ray, RayHit& hitInfo)
{
    // first transform ray to local space
    Vector3 relPos = ray.origin - position;
    Vector3 relDir = ray.direction;
    Vector3 relPos2D;
    Vector3 rayDir2D;

    relPos = relPos.ChangeBasis(localX, localY, localZ);
    relDir = relDir.ChangeBasis(localX, localY, localZ);

    // now we can project the ray onto the xz plane and solve a ray-circle intersection
    relPos2D = Vector3(relPos.x, 0, relPos.z);
    rayDir2D = Vector3(relDir.x, 0, relDir.z);

    Float a = rayDir2D.dot(rayDir2D);
    Float b = relPos2D.dot(rayDir2D);
    Float c = relPos2D.dot(relPos2D) - radius * radius;

    Float det2 = b * b - a * c;

    if (det2 < 0)
    {
        return false;
    }

    Float det = sqrt(det2);

    Float t1 = (-b - det) / a;
    Float t2 = (-b + det) / a;

    // based on signs of t1, t2, and the location of the ray origin, we can determine what the intersection type is

    // first, both negative means no intersection
    if (t1 < 0 && t2 < 0)
    {
        return false;
    }

    // both positive means we hit the cylinder from the outside
    if (t1 > 0 && t2 > 0)
    {
        bool hit = OutsideCylinderIntersection(ray, relPos, relDir, t1, hitInfo);
        if (hit)
        {
            hitInfo.inside = false;
        }
        return hit;
    }

    // now we know t1 is negative and t2 is positive, giving us 2 cases:
    // either we are inside the cylinder, or we started above/below the cylinder

    // first check for starting inside the cylinder
    if (relPos.y >= 0 && relPos.y <= height)
    {
        bool hit = InsideCylinderIntersection(ray, relPos, relDir, t2, hitInfo);
        if (hit)
        {
            hitInfo.inside = true;
        }
        return hit;
    }

    // we must be starting above/below the cylinder, in which case we just need to check
    // if we hit either of the caps

    if (relDir.y == 0)
    {
        return false;
    }

    Float t;
    Vector3 relHitPos;

    // first check top cap
    if (relDir.y < 0)
    {
        t = (height - relPos.y) / relDir.y;
        if (t < 0)
        {
            return false;
        }

        relHitPos = relPos + relDir * t;
        if (relHitPos.x * relHitPos.x + relHitPos.z * relHitPos.z <= radius * radius)
        {
            hitInfo.t = t;
            hitInfo.position = ray.origin + ray.direction * t;
            hitInfo.normal = localY;
            hitInfo.materialIndex = materialIndex;
            hitInfo.hit = true;
            hitInfo.inside = false;
            return true;
        }
    }

    // now check bottom cap
    if (relDir.y > 0)
    {
        t = (-relPos.y) / relDir.y;
        if (t < 0)
        {
            return false;
        }

        relHitPos = relPos + relDir * t;
        if (relHitPos.x * relHitPos.x + relHitPos.z * relHitPos.z <= radius * radius)
        {
            hitInfo.t = t;
            hitInfo.position = ray.origin + ray.direction * t;
            hitInfo.normal = -localY;
            hitInfo.materialIndex = materialIndex;
            hitInfo.hit = true;
            hitInfo.inside = false;
            return true;
        }
    }

    return false;
    
}

bool Cylinder::OutsideCylinderIntersection(Ray worldRay, Vector3 localRayOrigin, Vector3 localRayDir, Float t, RayHit& hitInfo)
{
    Vector3 relHitPos = localRayOrigin + localRayDir * t;
    // now check if it is within the height
    if (t > 0 && relHitPos.y >= 0 && relHitPos.y <= height)
    {
        hitInfo.t = t;
        hitInfo.normal = relHitPos;
        hitInfo.normal.y = 0;
        hitInfo.normal = hitInfo.normal.ChangeBasis(localXInv, localYInv, localZInv);
        hitInfo.normal.Normalize();
        hitInfo.position = worldRay.origin + worldRay.direction * t;
        hitInfo.materialIndex = materialIndex;
        hitInfo.hit = true;

        return true;
    }

    if (localRayDir.y == 0)
    {
        return false;
    }

    // now check if the ray hits the caps
    Float t_top = (height - localRayOrigin.y) / localRayDir.y;
    Float t_bottom = -localRayOrigin.y / localRayDir.y;

    // hit top plane if t_top is positive and the ray is pointing down
    if (t_top > 0 && localRayDir.y < 0)
    {
        relHitPos = localRayOrigin + localRayDir * t_top;
        // check if the hit point is within the radius
        if (relHitPos.x * relHitPos.x + relHitPos.z * relHitPos.z <= radius * radius)
        {
            hitInfo.t = t_top;
            hitInfo.normal = localY;
            hitInfo.position = worldRay.origin + worldRay.direction * t_top;
            hitInfo.materialIndex = materialIndex;
            hitInfo.hit = true;

            return true;
        }
    }

    // hit bottom plane if t_bottom is positive and the ray is pointing up
    if (t_bottom > 0 && localRayDir.y > 0)
    {
        relHitPos = localRayOrigin + localRayDir * t_bottom;
        // check if the hit point is within the radius
        if (relHitPos.x * relHitPos.x + relHitPos.z * relHitPos.z <= radius * radius)
        {
            hitInfo.t = t_bottom;
            hitInfo.normal = -localY;
            hitInfo.position = worldRay.origin + worldRay.direction * t_bottom;
            hitInfo.materialIndex = materialIndex;
            hitInfo.hit = true;

            return true;
        }
    }

    return false;
}

bool Cylinder::InsideCylinderIntersection(Ray worldRay, Vector3 localRayOrigin, Vector3 localRayDir, Float t, RayHit& hitInfo)
{
    Vector3 relHitPos = localRayOrigin + localRayDir * t;
    // check if we hit the side
    if (t > 0 && relHitPos.y >= 0 && relHitPos.y <= height)
    {
        hitInfo.t = t;
        hitInfo.normal = -relHitPos;
        hitInfo.normal.y = 0;
        hitInfo.normal = hitInfo.normal.ChangeBasis(localXInv, localYInv, localZInv);
        hitInfo.normal.Normalize();
        hitInfo.position = worldRay.origin + worldRay.direction * t;
        hitInfo.materialIndex = materialIndex;
        hitInfo.hit = true;

        return true;
    }

    // make sure localRayDir.y isn't 0 (if it is, something went wrong)
    if (localRayDir.y == 0)
    {
        return false;
    }

    // then we must have hit the top or bottom cap
    // first check top cap
    if (localRayDir.y > 0)
    {
        t = (height - localRayOrigin.y) / localRayDir.y;
        if (t < 0)
        {
            return false; // this should never happen
        }
        hitInfo.t = t;
        hitInfo.normal = -localY;
        hitInfo.position = worldRay.origin + worldRay.direction * t;
        hitInfo.materialIndex = materialIndex;
        hitInfo.hit = true;

        return true;
    }

    // then check bottom cap
    if (localRayDir.y < 0)
    {
        t = -localRayOrigin.y / localRayDir.y;
        if (t < 0)
        {
            return false; // this should never happen
        }
        hitInfo.t = t;
        hitInfo.normal = localY;
        hitInfo.position = worldRay.origin + worldRay.direction * t;
        hitInfo.materialIndex = materialIndex;
        hitInfo.hit = true;

        return true;
    }

    return false; // this should never happen

}

WorldBounds Cylinder::GetWorldBounds()
{
    Vector3 min, max;
    Vector3 top1, top2, top3, top4;
    Vector3 bottom1, bottom2, bottom3, bottom4;
    top1 = position + localY * height + localX * radius;
    top2 = position + localY * height + localZ * radius;
    top3 = position + localY * height - localX * radius;
    top4 = position + localY * height - localZ * radius;
    bottom1 = position + localX * radius;
    bottom2 = position + localZ * radius;
    bottom3 = position - localX * radius;
    bottom4 = position - localZ * radius;

    Vector3 top = Vector3::Min(Vector3::Min(top1, top2), Vector3::Min(top3, top4));
    Vector3 bottom = Vector3::Min(Vector3::Min(bottom1, bottom2), Vector3::Min(bottom3, bottom4));
    min = Vector3::Min(top, bottom);

    top = Vector3::Max(Vector3::Max(top1, top2), Vector3::Max(top3, top4));
    bottom = Vector3::Max(Vector3::Max(bottom1, bottom2), Vector3::Max(bottom3, bottom4));
    max = Vector3::Max(top, bottom);
    return WorldBounds(min, max);
}

void Cylinder::SetSideUVInfo(RayHit& hitInfo, Vector3 localHitPos)
{
    // calculate the uv coordinates
    Float u = atan2(localHitPos.z, localHitPos.x);
    u = 1 - (u + M_PI) / (2 * M_PI);
    Float v = localHitPos.y / height;

    hitInfo.uv.u = u;
    hitInfo.uv.v = v;

    // calculate the tangent and bitangent
    Vector3 tangent = Vector3(localHitPos.z, 0, -localHitPos.x);
    tangent = tangent.ChangeBasis(localXInv, localYInv, localZInv);
    Vector3 bitangent = hitInfo.normal.cross(tangent);

    hitInfo.tangent = tangent.normalized();
    hitInfo.bitangent = bitangent.normalized();
}

void Cylinder::SetCapUVInfo(RayHit& hitInfo, bool top)
{
    // for caps we'll just set v to 0 or 1
    Float v = top ? 1 : 0;
    Float u = atan2(hitInfo.normal.z, hitInfo.normal.x) / (M_PI) + 0.5f;

    hitInfo.uv.u = u;
    hitInfo.uv.v = v;

    // normal is along localY, so we can use localX and Z for tangent and bitangent
    hitInfo.tangent = top ? localX : -localX;
    hitInfo.bitangent = localZ;
}