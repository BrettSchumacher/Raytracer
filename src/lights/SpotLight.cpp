#include "SpotLight.h"

using namespace std;

SpotLight::SpotLight()
{
    position = Vector3(0.0, 0.0, 0.0);
    direction = Vector3(0.0, -1.0, 0.0);
    innerAngle = 0.0;
    outerAngle = 0.0;
    innerAngleCos = 1.0;
    outerAngleCos = 1.0;
}

SpotLight::SpotLight(Vector3 color)
{
    position = Vector3(0.0, 0.0, 0.0);
    direction = Vector3(0.0, -1.0, 0.0);
    innerAngle = 0.0;
    outerAngle = 0.0;
    innerAngleCos = 1.0;
    outerAngleCos = 1.0;
    this->color = color;
    c1 = 1.0;
    c2 = 0.0;
    c3 = 0.0;
}

SpotLight::SpotLight(Vector3 position, Vector3 direction, Vector3 color, Float innerAngle)
{
    this->position = position;
    this->direction = direction.normalized();
    this->innerAngle = innerAngle;
    this->outerAngle = innerAngle;
    this->color = color;
    this->c1 = 1.0;
    this->c2 = 0.0;
    this->c3 = 0.0;

    // inner/outer angle measures entire opening angle of cone
    // so for their cos we want to divide by 2 since angle with points will be with respect to center of the cone
    innerAngleCos = cos(innerAngle * 0.5 * M_PI / 180.0);
    outerAngleCos = cos(outerAngle * 0.5 * M_PI / 180.0);
}

SpotLight::SpotLight(Vector3 position, Vector3 direction, Vector3 color, Float innerAngle, Float outerAngle, Float c1, Float c2, Float c3)
{
    if (outerAngle < innerAngle)
    {
        cout << "WARNING: outer angle must be greater than inner angle, setting outer angle to inner angle" << endl;
        outerAngle = innerAngle;
    }

    this->position = position;
    this->direction = direction.normalized();
    this->innerAngle = innerAngle;
    this->outerAngle = outerAngle;
    this->color = color;
    this->c1 = c1;
    this->c2 = c2;
    this->c3 = c3;

    innerAngleCos = cos(innerAngle * 0.5 * M_PI / 180.0);
    outerAngleCos = cos(outerAngle * 0.5 * M_PI / 180.0);
}

Float SpotLight::GetIntensityAt(Vector3 Point)
{
    Vector3 dirTo = GetDirectionAt(Point);
    Float cosAngle = dirTo.dot(direction);
    Float relIntensity;
    
    // cosAngle will be 1 if in center of light
    if (cosAngle >= innerAngleCos) 
    {
        // inside inner cone
        relIntensity = 1.0;
    }
    else if (cosAngle >= outerAngleCos)
    {
        // inside outer cone, but not inside inner cone
        relIntensity = (cosAngle - outerAngleCos) / (innerAngleCos - outerAngleCos);
    }
    else
    {
        // outside outer cone
        relIntensity = 0.0;
    }

    Float dist = GetDistanceFrom(Point);
    Float atten = 1.0 / (c1 + c2 * dist + c3 * dist * dist);
    
    return relIntensity * atten;
}

Vector3 SpotLight::GetDirectionAt(Vector3 point)
{
    return (point - position).normalized();
}

Float SpotLight::GetDistanceFrom(Vector3 point)
{
    return (position - point).magnitude();
}