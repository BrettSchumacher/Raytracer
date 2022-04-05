#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight : public Light
{
    public:
        Vector3 position;
        Float c1, c2, c3; // attenuation coefficients

        PointLight();
        PointLight(Vector3 color);
        PointLight(Vector3 position, Vector3 color, Float c1 = 1.0, Float c2 = 0.0, Float c3 = 0.0);
        ~PointLight() {};

        Float GetIntensityAt(Vector3 point);
        Vector3 GetDirectionAt(Vector3 point);
        Float GetDistanceFrom(Vector3 point);
};

#endif