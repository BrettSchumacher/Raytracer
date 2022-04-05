#include "Vector3.h"

using namespace std;

Float Vector3::dot(Vector3 v)
{
    return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::cross(Vector3 v)
{
    return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

Vector3 Vector3::normalized()
{
    Float mag = magnitude();
    if (mag == 0)
    {
        return Vector3(0, 0, 0);
    }
    return Vector3(x / mag, y / mag, z / mag);
}

void Vector3::Normalize()
{
    Float mag = magnitude();

    if (mag == 0)
    {
        x = 0;
        y = 0;
        z = 0;
    }
    else
    {
        x /= mag;
        y /= mag;
        z /= mag;
    }

    UpdateMagnitude();
}

Float Vector3::magnitude()
{
    if (x != lastX || y != lastY || z != lastZ)
    {
        lastMagnitude = sqrt(x * x + y * y + z * z);
        lastX = x;
        lastY = y;
        lastZ = z;
    }

    return lastMagnitude;
}

Float Vector3::sqrMagnitude()
{
    return x * x + y * y + z * z;
}

void Vector3::UpdateMagnitude()
{
    lastMagnitude = sqrt(x * x + y * y + z * z);
    lastX = x;
    lastY = y;
    lastZ = z;
}

// assumes that the new basis is orthonormal
Vector3 Vector3::ChangeBasis(Vector3 newX, Vector3 newY, Vector3 newZ)
{
    Vector3 newVec;
    newVec.x = dot(newX);
    newVec.y = dot(newY);
    newVec.z = dot(newZ);
    return newVec;
}

Float Vector3::Distance(Vector3 v1, Vector3 v2)
{
    return (v1 - v2).magnitude();
}

Float Vector3::Angle(Vector3 v1, Vector3 v2)
{
    return acos(v1.dot(v2) / (v1.magnitude() * v2.magnitude()));
}

Vector3 Vector3::Project(Vector3 v, Vector3 on)
{
    Vector3 norm = on.normalized();
    return norm * v.dot(norm);
}

Vector3 Vector3::ProjectOnPlane(Vector3 v, Vector3 normal)
{
    return v - Project(v, normal);
}

Vector3 Vector3::RandOnUnitSphere()
{
    Float x = (rand() / ((Float) RAND_MAX)) * 2 - 1;
    Float y = (rand() / ((Float) RAND_MAX)) * 2 - 1;
    Float z = (rand() / ((Float) RAND_MAX)) * 2 - 1;

    while (x * x + y * y + z * z > 1)
    {
        x = (rand() / ((Float) RAND_MAX)) * 2 - 1;
        y = (rand() / ((Float) RAND_MAX)) * 2 - 1;
        z = (rand() / ((Float) RAND_MAX)) * 2 - 1;
    }

    return Vector3(x, y, z).normalized();
}

Vector3 Vector3::RandVecAroundNorm(Vector3 norm)
{
    Vector3 vec = Vector3::RandOnUnitSphere();
    while (vec.dot(norm) < 0)
    {
        vec = Vector3::RandOnUnitSphere();
    }

    return vec;
}

Vector3 Vector3::Max(const Vector3& v1, const Vector3& v2)
{
    return Vector3(max(v1.x, v2.x), max(v1.y, v2.y), max(v1.z, v2.z));
}

Vector3 Vector3::Min(const Vector3& v1, const Vector3& v2)
{
    return Vector3(min(v1.x, v2.x), min(v1.y, v2.y), min(v1.z, v2.z));
}

const Vector3 Vector3::zero = Vector3(0, 0, 0);
const Vector3 Vector3::one = Vector3(1, 1, 1);
const Vector3 Vector3::up = Vector3(0, 1, 0);
const Vector3 Vector3::forward = Vector3(0, 0, 1);
const Vector3 Vector3::right = Vector3(1, 0, 0);

Vector3 Vector3::operator+(const Vector3& v)
{
    return Vector3(x + v.x, y + v.y, z + v.z);
}

Vector3 Vector3::operator-(const Vector3& v)
{
    return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator*(const float& f)
{
    return Vector3(x * f, y * f, z * f);
}

Vector3 Vector3::operator*(const Vector3& v)
{
    return Vector3(x * v.x, y * v.y, z * v.z);
}

Vector3 Vector3::operator/(const float& f)
{
    return Vector3(x / f, y / f, z / f);
}

Vector3& Vector3::operator+=(const Vector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vector3& Vector3::operator*=(const float& f)
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

Vector3& Vector3::operator*=(const Vector3& v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
}

Vector3& Vector3::operator/=(const float& f)
{
    x /= f;
    y /= f;
    z /= f;
    return *this;
}

bool Vector3::operator==(const Vector3& v)
{
    return x == v.x && y == v.y && z == v.z;
}

bool Vector3::operator!=(const Vector3& v)
{
    return !(this == &v);
}

Vector3& Vector3::operator=(const Vector3& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

string Vector3::ToString()
{
    return "(" + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ")";
}

ostream& operator<<(ostream& os, const Vector3& v)
{
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

Vector3 operator*(const float& f, const Vector3& v)
{
    return Vector3(v.x * f, v.y * f, v.z * f);
}

Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
    return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}


UV UV::operator+(const UV& uv) const
{
    return UV(u + uv.u, v + uv.v);
}

UV UV::operator-(const UV& uv) const
{
    return UV(u - uv.u, v - uv.v);
}

UV UV::operator-() const
{
    return UV(-u, -v);
}

UV UV::operator*(const float& f) const
{
    return UV(u * f, v * f);
}

UV UV::operator/(const float& f) const
{
    return UV(u / f, v / f);
}

UV operator*(const float& f, const UV& uv)
{
    return UV(uv.u * f, uv.v * f);
}