#ifndef SHAPE_H
#define SHAPE_H

#include "math/Vector3.h"
#include "core/Material.h"
#include "core/Ray.h"


struct WorldBounds
{
    Vector3 min;
    Vector3 max;
    Vector3 centroid;

    WorldBounds()
    {
        min = Vector3(0, 0, 0);
        max = Vector3(0, 0, 0);
    }

    WorldBounds(Vector3 min, Vector3 max)
    {
        this->min = min;
        this->max = max;
        this->centroid = (min + max) / 2;
    }

    // morton code is linearization of 3d space by interleaving bits of x, y, and z of the centroid
    long GetMortonCode(Vector3 offset = Vector3::zero, Float scaleFac = 1);
};


class Shape
{
    public:
        int materialIndex;
        int id;
        bool selfShadowing;
        
        Shape();
        virtual ~Shape() {};

        virtual bool Intersect(Ray ray, RayHit& hitInfo) = 0; 
        virtual WorldBounds GetWorldBounds() = 0;
        virtual bool IgnoreSelfShadowing() = 0;

};  

#endif