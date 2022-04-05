#include "PointLight.h"

PointLight::PointLight()
{
    position = Vector3(0.0, 0.0, 0.0);
    c1 = 1.0;
    c2 = 0.0;
    c3 = 0.0;
}

PointLight::PointLight(Vector3 color)
{
    this->position = Vector3(0.0, 0.0, 0.0);
    this->color = color;
    this->c1 = 1.0;
    this->c2 = 0.0;
    this->c3 = 0.0;
}

PointLight::PointLight(Vector3 position, Vector3 color, Float c1, Float c2, Float c3)
{
    this->position = position;
    this->color = color;
    this->c1 = c1;
    this->c2 = c2;
    this->c3 = c3;
}

Vector3 PointLight::GetDirectionAt(Vector3 point)
{
    return (point - position).normalized();
}

Float PointLight::GetIntensityAt(Vector3 point)
{
    Float dist = GetDistanceFrom(point);
    Float dist2 = dist * dist;
    Float inten = 1.0 / (c1 + c2 * dist + c3 * dist2);
    return inten;
}

Float PointLight::GetDistanceFrom(Vector3 point)
{
    return (position - point).magnitude();
}