#include "Ray.h"

Ray::Ray()
{
    origin = Vector3(0, 0, 0);
    direction = Vector3(0, 0, 0);
    iors.push_back(1);
}

Ray::Ray(Vector3 origin, Vector3 direction, Float ior)
{
    this->origin = origin;
    this->direction = direction.normalized();
    iors.push_back(ior);
}