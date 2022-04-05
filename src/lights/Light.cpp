#include "Light.h"

Light::Light()
{
    color = Vector3(1.0, 1.0, 1.0);
}

Light::Light(Vector3 color)
{
    this->color = color;
}