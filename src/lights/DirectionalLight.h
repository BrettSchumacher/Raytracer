#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"

class DirectionalLight : public Light
{
    public:
        Vector3 direction;

        DirectionalLight();
        DirectionalLight(Vector3 color);
        DirectionalLight(Vector3 direction, Vector3 color);
        ~DirectionalLight() {};

        Float GetIntensityAt(Vector3 point);
        Vector3 GetDirectionAt(Vector3 point);
        Float GetDistanceFrom(Vector3 point);
};

#endif