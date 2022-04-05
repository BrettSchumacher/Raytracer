#ifndef RAY_H
#define RAY_H

#include "math/Vector3.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

using namespace std;

struct Ray 
{
    Vector3 origin;
    Vector3 direction;
    vector<Float> iors;

    Ray();

    Ray(Vector3 origin, Vector3 direction, Float ior = 1);
};

struct RayHit
{
    bool hit = false;
    Float t = INFINITY;
    Vector3 normal;
    Vector3 position;
    Vector3 tangent;
    Vector3 bitangent;
    UV uv;
    bool inside = false;
    int materialIndex = 0;
    int shapeIndex = 0;

    // logic operators for comparing RayHits
    operator bool() const { return hit; }
    bool operator<(const RayHit& rhs) const { return t < rhs.t; }
    bool operator<=(const RayHit& rhs) const { return t <= rhs.t; }
    bool operator>(const RayHit& rhs) const { return t > rhs.t; }
    bool operator>=(const RayHit& rhs) const { return t >= rhs.t; }
    bool operator==(const RayHit& rhs) const { return t == rhs.t; }
    bool operator!=(const RayHit& rhs) const { return t != rhs.t; }

};

#endif