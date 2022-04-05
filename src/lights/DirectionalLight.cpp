#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
    direction = Vector3(0.0, -1.0, 0.0);
}

DirectionalLight::DirectionalLight(Vector3 color)
{
    direction = Vector3(0.0, -1.0, 0.0);
    this->color = color;
}

DirectionalLight::DirectionalLight(Vector3 direction, Vector3 color)
{
    this->direction = direction.normalized();
    this->color = color;
}

Float DirectionalLight::GetIntensityAt(Vector3 point)
{
    return 1.0;
}

Vector3 DirectionalLight::GetDirectionAt(Vector3 point)
{
    return direction;
}

Float DirectionalLight::GetDistanceFrom(Vector3 point)
{
    return INFINITY;
}