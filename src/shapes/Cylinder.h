#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"
#include <math.h>

class Cylinder : public Shape
{
    public:
        Vector3 position;
        Float radius;
        Float height;

        Cylinder();
        Cylinder(Vector3 position, Vector3 upDir, int matInd, Float radius, Float height);
        ~Cylinder() {};

        bool Intersect(Ray ray, RayHit& hitInfo);
        WorldBounds GetWorldBounds();
        bool IgnoreSelfShadowing() { return false; }

        void SetUpDir(Vector3 upDir);
        Vector3 GetUpDir();

    private:
        Vector3 localX, localY, localZ, localXInv, localYInv, localZInv; // basis vectors for changing to and from local space

        void CalculateLocalBasis();
        bool OutsideCylinderIntersection(Ray worldRay, Vector3 localRayOrigin, Vector3 localRayDir, Float t, RayHit& hitInfo);
        bool InsideCylinderIntersection(Ray worldRay, Vector3 localRayOrigin, Vector3 localRayDir, Float t, RayHit& hitInfo);

        void SetSideUVInfo(RayHit& hitInfo, Vector3 localHitPos);
        void SetCapUVInfo(RayHit& hitInfo, bool top);
};


#endif