#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "Light.h"

class SpotLight : public Light
{
    public:
        Vector3 position;
        Vector3 direction;
        Float innerAngle;
        Float outerAngle;
        Float c1, c2, c3; // attenuation coefficients

        SpotLight();
        SpotLight(Vector3 color);
        SpotLight(Vector3 position, Vector3 direction, Vector3 color, Float innerAngle);
        SpotLight(Vector3 position, Vector3 direction, Vector3 color, Float innerAngle, Float outerAngle, Float c1 = 1.0, Float c2 = 0.0, Float c3 = 0.0);
        ~SpotLight() {};

        Float GetIntensityAt(Vector3 point);
        Vector3 GetDirectionAt(Vector3 point);
        Float GetDistanceFrom(Vector3 point);

    private:
        Float innerAngleCos; // allows us to just use dot product instead of doing a full acos
        Float outerAngleCos;
};

#endif