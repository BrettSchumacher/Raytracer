#ifndef QUATERNION
#define QUATERNION

#include "Vector3.h"

#include <math.h>

class Quaternion
{
    public:
        Float w, x, y, z;

        Quaternion();
        Quaternion(Float w, Float x, Float y, Float z);
        Quaternion(const Vector3& eulers);
        Quaternion(const Quaternion& q);

        Vector3 getEulerAngles() const;
        void setEulerAngles(const Vector3& eulerAngles);
        Quaternion conjugate() const;
        Quaternion inverse() const;

        static const Quaternion identity;

        Quaternion operator+(const Quaternion& rhs) const;
        Quaternion operator-(const Quaternion& rhs) const;
        Quaternion operator-() const;
        Quaternion operator*(const Quaternion& rhs) const;
        Quaternion operator*(const Float& f) const;
        Quaternion operator/(const Float& f) const;
        Quaternion& operator+=(const Quaternion& rhs);
        Quaternion& operator-=(const Quaternion& rhs);
        Quaternion& operator*=(const Quaternion& rhs);
        Quaternion& operator*=(const Float& f);
        Quaternion& operator/=(const Float& f);
        Quaternion& operator=(const Quaternion& rhs);
        Vector3 operator*(const Vector3& v) const;

        friend Quaternion operator*(const Float& f, const Quaternion& v);
        friend ostream& operator<<(ostream& os, const Quaternion& q);

    private:
};


#endif