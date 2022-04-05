#include "Triangle.h"

Triangle::Triangle()
{
    v0 = Vector3(0, 0, 0);
    v1 = Vector3(0, 0, 0);
    v2 = Vector3(0, 0, 0);
    materialIndex = 0;
}

Triangle::Triangle(Vector3 v0, Vector3 v1, Vector3 v2, int matInd)
{
    this->v0 = v0;
    this->v1 = v1;
    this->v2 = v2;
    materialIndex = matInd;

    Initialize();
}

Triangle::Triangle(Vector3 v0, Vector3 v1, Vector3 v2, Vector3 n0, Vector3 n1, Vector3 n2, int matInd)
{
    this->v0 = v0;
    this->v1 = v1;
    this->v2 = v2;
    vn0 = n0;
    vn1 = n1;
    vn2 = n2;
    hasNormals = true;
    materialIndex = matInd;

    Initialize();
}

Triangle::Triangle(Vector3 v0, Vector3 v1, Vector3 v2, Vector3 n0, Vector3 n1, Vector3 n2, UV uv0, UV uv1, UV uv2, int matInd)
{
    this->v0 = v0;
    this->v1 = v1;
    this->v2 = v2;
    vn0 = n0;
    vn1 = n1;
    vn2 = n2;
    this->uv0 = uv0;
    this->uv1 = uv1;
    this->uv2 = uv2;
    hasNormals = true;
    hasUVs = true;
    materialIndex = matInd;

    Initialize();
}

Triangle::Triangle(Vector3 v0, Vector3 v1, Vector3 v2, UV uv0, UV uv1, UV uv2, int matInd)
{
    this->v0 = v0;
    this->v1 = v1;
    this->v2 = v2;
    this->uv0 = uv0;
    this->uv1 = uv1;
    this->uv2 = uv2;
    hasUVs = true;
    materialIndex = matInd;

    Initialize();
}

void Triangle::Initialize()
{
    // calculate normal
    e1 = v1 - v0;
    e2 = v2 - v0;
    normal = e1.cross(e2).normalized();

    // calculate constants for barycentric coords
    d11 = e1.dot(e1);
    d12 = e1.dot(e2);
    d22 = e2.dot(e2);
    D = d11 * d22 - d12 * d12;
}

void Triangle::SetNormals(Vector3 n0, Vector3 n1, Vector3 n2)
{
    vn0 = n0;
    vn1 = n1;
    vn2 = n2;
    hasNormals = true;
}

void Triangle::SetUVs(UV uv0, UV uv1, UV uv2)
{
    this->uv0 = uv0;
    this->uv1 = uv1;
    this->uv2 = uv2;
    hasUVs = true;
}

// calculate the intersection point of the ray and the plane
bool Triangle::PlaneIntersection(Ray ray, Vector3& point)
{
    Float denom = normal.dot(ray.direction);
    if (abs(denom) == 0)
    {
        return false;
    }

    Float t = normal.dot(v0 - ray.origin) / denom;
    if (t < 0)
    {
        return false;
    }

    point = ray.origin + ray.direction * t;
    return true;
}

// get barycentric coords for point on plane of triangle
bool Triangle::GetBarycentricCoords(Vector3 point, Float& a, Float& b, Float& g)
{
    if (abs(D) == 0)
    {
        return false;
    }

    Vector3 ep = point - v0;
    Float dp1 = e1.dot(ep);
    Float dp2 = e2.dot(ep);
    Float Db = d22 * dp1 - d12 * dp2;
    Float Dg = d11 * dp2 - d12 * dp1;

    Float B = Db / D;
    Float G = Dg / D;

    if (B + G > 1.0 || B < 0.0 || G < 0.0)
    {
        return false;
    }
    else
    {
        a = 1.0 - B - G;
        b = B;
        g = G;
        return true;
    }
}

bool Triangle::Intersect(Ray ray, RayHit& hitInfo)
{
    Vector3 point;
    if (!PlaneIntersection(ray, point))
    {
        return false;
    }

    Float a, b, g;
    if (!GetBarycentricCoords(point, a, b, g))
    {
        return false;
    }

    hitInfo.position = point;
    hitInfo.materialIndex = materialIndex;
    hitInfo.t = (point - ray.origin).magnitude();
    hitInfo.hit = true;
    hitInfo.inside = normal.dot(ray.direction) > 0; // if ray direction and normal are aligned, then we are inside the triangle

    if (hasNormals)
    {
        hitInfo.normal = a * vn0 + b * vn1 + g * vn2;
    }
    else
    {
        hitInfo.normal = normal;
    }

    SetUVInfo(hitInfo, Vector3(a, b, g));

    return true;
}

WorldBounds Triangle::GetWorldBounds()
{
    Vector3 min = Vector3::Min(v0, Vector3::Min(v1, v2));
    Vector3 max = Vector3::Max(v0, Vector3::Max(v1, v2));
    return WorldBounds(min, max);
}

void Triangle::SetUVInfo(RayHit& hitInfo, Vector3 barys)
{
    if (hasUVs)
    {
        hitInfo.uv = barys.x * uv0 + barys.y * uv1 + barys.z * uv2;

        Float du1 = uv1.u - uv0.u;
        Float dv1 = uv1.v - uv0.v;
        Float du2 = uv2.u - uv0.u;
        Float dv2 = uv2.v - uv0.v;

        Float det = du1 * dv2 - dv1 * du2;
        if (abs(det) < 0.001)
        {
            hitInfo.tangent = e1.normalized();
            hitInfo.bitangent = normal.cross(hitInfo.tangent).normalized();
            return;
        }

        Float invDet = 1.0 / det;
        Vector3 tangent = invDet * (dv2 * e1 - dv1 * e2);
        Vector3 bitangent = invDet * (-du2 * e1 + du1 * e2);

        hitInfo.tangent = tangent.normalized();
        hitInfo.bitangent = bitangent.normalized();
    }
    else
    {
        // default to (0,0) for uv0, (1,0) for uv1, (0,1) for uv2
        hitInfo.uv = barys.y * UV(1, 0) + barys.z * UV(0, 1);

        // default for tangent along e1
        hitInfo.tangent = e1.normalized();
        hitInfo.bitangent = normal.cross(hitInfo.tangent).normalized();
    }


}