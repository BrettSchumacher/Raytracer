#include "Quaternion.h"

const Quaternion Quaternion::identity = Quaternion(1, 0, 0, 0);

Quaternion::Quaternion()
{
    w = 1;
    x = 0;
    y = 0;
    z = 0;
}

Quaternion::Quaternion(Float w, Float x, Float y, Float z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

Quaternion::Quaternion(const Vector3& eulers)
{
    setEulerAngles(eulers);
}

Quaternion::Quaternion(const Quaternion& q)
{
    w = q.w;
    x = q.x;
    y = q.y;
    z = q.z;
}

Vector3 Quaternion::getEulerAngles() const
{
    Float yaw = atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
    Float pitch = asin(2 * (w * y - z * x));
    Float roll = atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));

    return Vector3(pitch, yaw, roll);
}

// quaternion wiki helpin me out: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_conversion
void Quaternion::setEulerAngles(const Vector3& eulerAngles)
{
    Float roll = eulerAngles.x;
    Float pitch = eulerAngles.y;
    Float yaw = eulerAngles.z;

    Float cy = cos(yaw * 0.5);
    Float sy = sin(yaw * 0.5);
    Float cp = cos(pitch * 0.5);
    Float sp = sin(pitch * 0.5);
    Float cr = cos(roll * 0.5);
    Float sr = sin(roll * 0.5);

    w = cr * cp * cy + sr * sp * sy;
    x = sr * cp * cy - cr * sp * sy;
    y = cr * sp * cy + sr * cp * sy;
    z = cr * cp * sy - sr * sp * cy;
}

Quaternion Quaternion::conjugate() const
{
    return Quaternion(w, -x, -y, -z);
}

Quaternion Quaternion::inverse() const
{
    return conjugate() / (w * w + x * x + y * y + z * z);
}

Quaternion Quaternion::operator+(const Quaternion& rhs) const
{
    return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
}

Quaternion Quaternion::operator-(const Quaternion& rhs) const
{
    return Quaternion(w - rhs.w, x - rhs.x, y - rhs.y, z - rhs.z);
}

Quaternion Quaternion::operator-() const
{
    return Quaternion(-w, -x, -y, -z);
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
    return Quaternion(w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
                      w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
                      w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
                      w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x);
}

Quaternion Quaternion::operator*(const Float& f) const
{
    return Quaternion(w * f, x * f, y * f, z * f);
}

Vector3 Quaternion::operator*(const Vector3& v) const
{
    Quaternion vq(0, v.x, v.y, v.z);
    Quaternion result = (*this * vq) * conjugate();

    return Vector3(result.x, result.y, result.z);
}

Quaternion Quaternion::operator/(const Float& f) const
{
    return Quaternion(w / f, x / f, y / f, z / f);
}

Quaternion& Quaternion::operator+=(const Quaternion& rhs)
{
    w += rhs.w;
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& rhs)
{
    w -= rhs.w;
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& rhs)
{
    *this = *this * rhs;
    return *this;
}

Quaternion& Quaternion::operator*=(const Float& f)
{
    w *= f;
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

Quaternion& Quaternion::operator/=(const Float& f)
{
    w /= f;
    x /= f;
    y /= f;
    z /= f;
    return *this;
}

Quaternion& Quaternion::operator=(const Quaternion& rhs)
{
    w = rhs.w;
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
}

Quaternion operator*(const Float& f, const Quaternion& v)
{
    return Quaternion(f * v.w, f * v.x, f * v.y, f * v.z);
}

ostream& operator<<(ostream& os, const Quaternion& q)
{
    os << "(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
    return os;
}