#ifndef LIGHT_H
#define LIGHT_H

#include "math/Vector3.h"
#define _USE_MATH_DEFINES
#include <math.h>

class Light
{
    public:
        Vector3 color;

        Light();
        Light(Vector3 color);
        virtual ~Light() {};

        virtual Float GetIntensityAt(Vector3 point) = 0;
        virtual Vector3 GetDirectionAt(Vector3 point) = 0;
        virtual Float GetDistanceFrom(Vector3 point) = 0;
};

#endif