#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Shape.h"

class Triangle : public Shape
{
    public:
        Triangle();
        Triangle(Vector3 v0, Vector3 v1, Vector3 v2, int matInd);
        Triangle(Vector3 v0, Vector3 v1, Vector3 v2, Vector3 n0, Vector3 n1, Vector3 n2, int matInd);
        Triangle(Vector3 v0, Vector3 v1, Vector3 v2, Vector3 n0, Vector3 n1, Vector3 n2, UV uv0, UV uv1, UV uv2, int matInd);
        Triangle(Vector3 v0, Vector3 v1, Vector3 v2, UV uv0, UV uv1, UV uv2, int matInd);
        ~Triangle() {};

        bool Intersect(Ray ray, RayHit& hitInfo);
        WorldBounds GetWorldBounds();
        bool IgnoreSelfShadowing() { return false; }

        void SetNormals(Vector3 n0, Vector3 n1, Vector3 n2);
        void SetUVs(UV uv0, UV uv1, UV uv2);

        bool PlaneIntersection(Ray ray, Vector3& point);
        bool GetBarycentricCoords(Vector3 point, Float& a, Float& b, Float& g);

    private:
        Vector3 v0, v1, v2, e1, e2, normal;
        Vector3 vn0, vn1, vn2;
        UV uv0, uv1, uv2;
        bool hasNormals = false;
        bool hasUVs = false;
        Float d11, d12, d22, D; // constants for barycentric coords

        void Initialize();
        void SetUVInfo(RayHit& hitInfo, Vector3 baryCoords);
};

#endif