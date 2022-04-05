#ifndef VECTOR3_H
#define VECTOR3_H

#include <string>
#include <iostream>
#include <math.h>

// find a better place to put this later
// putting it here for now cuz it's the head of the dependency chain
#ifdef USE_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif

using namespace std;

struct UV
{
    Float u;
    Float v;

    UV() : u(0), v(0) {}
    UV(Float u, Float v) : u(u), v(v) {}

    UV operator+(const UV& rhs) const;
    UV operator-(const UV& rhs) const;
    UV operator-() const;
    UV operator*(const float& f) const;
    UV operator/(const float& f) const;
    friend UV operator*(const float& f, const UV& v);

};

struct Vector3
{
    Float x, y, z;
    Vector3() : x(0), y(0), z(0) { lastX = lastY = lastZ = lastMagnitude = 0; };
    Vector3(Float x, Float y, Float z) : x(x), y(y), z(z) { lastX = lastY = lastZ = lastMagnitude = 0; };
    Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z) { lastX = lastY = lastZ = lastMagnitude = 0; };

    Float dot(Vector3 v);
    Vector3 cross(Vector3 v);
    Vector3 normalized();
    void Normalize();
    Float magnitude();
    Float sqrMagnitude();
    Vector3 ChangeBasis(Vector3 newX, Vector3 newY, Vector3 newZ);

    static Float Distance(Vector3 v1, Vector3 v2);
    static Float Angle(Vector3 v1, Vector3 v2);
    static Vector3 Project(Vector3 v, Vector3 on);
    static Vector3 ProjectOnPlane(Vector3 v, Vector3 normal);
    static Vector3 RandOnUnitSphere();
    static Vector3 RandVecAroundNorm(Vector3 norm);
    static Vector3 Max(const Vector3& v1, const Vector3& v2);
    static Vector3 Min(const Vector3& v1, const Vector3& v2);
    static const Vector3 zero;
    static const Vector3 one;
    static const Vector3 up;
    static const Vector3 forward;
    static const Vector3 right;
    
    Vector3 operator+(const Vector3& v);
    Vector3 operator-(const Vector3& v);
    Vector3 operator-() const;
    Vector3 operator*(const float& f);
    Vector3 operator*(const Vector3& v);
    Vector3 operator/(const float& f);
    Vector3& operator+=(const Vector3& v);
    Vector3& operator-=(const Vector3& v);
    Vector3& operator*=(const float& f);
    Vector3& operator*=(const Vector3& v);
    Vector3& operator/=(const float& f);
    bool operator==(const Vector3& v);
    bool operator!=(const Vector3& v);
    Vector3& operator=(const Vector3& v);
    
    string ToString();
    string PrintRGB() { return to_string((int)(x*255.99)) + " " + to_string((int)(y*255.99)) + " " + to_string((int)(z*255.99)); }
    friend ostream& operator<<(ostream& os, const Vector3& v);
    friend Vector3 operator*(const float& f, const Vector3& v);
    friend Vector3 operator-(const Vector3& v1, const Vector3& v2);
        
    private:
        Float lastMagnitude;
        Float lastX, lastY, lastZ;

        void UpdateMagnitude();
};

#endif