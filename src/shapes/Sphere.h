#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include <math.h>

class Sphere : public Shape
{
    public:
        Vector3 position;
        Float radius;

        Sphere();
        Sphere(Vector3 position, int matInd, Float radius);
        ~Sphere() {};

        bool Intersect(Ray ray, RayHit& hitInfo);
        WorldBounds GetWorldBounds();
        bool IgnoreSelfShadowing() { return false; }

    private:
        void SetUVInfo(RayHit& hitInfo);
};

#endif