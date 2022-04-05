#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vector3.h"
#include "Quaternion.h"

class Transform
{
    public:
        Transform();
        Transform(const Vector3& position, const Quaternion& rotation);
        Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
        Transform(const Transform& transform);
        ~Transform();

        Vector3 getPosition() const;
        Vector3 getScale() const;
        Vector3 getEulerAngles() const;
        Quaternion getRotation() const;

        void setPosition(const Vector3& pos);
        void setScale(const Vector3& scale);
        void setRotation(const Quaternion& rot);
        void setRotation(const Vector3& eulerAngles);

        void translate(const Vector3& translation);
        void rotate(const Quaternion& rotation);
        void rotate(const Vector3& eulerAngles);
        void dialate(const Vector3& scale);

        // transform point from local space to world space
        Vector3 transformPoint(const Vector3& point) const;
        Vector3 transformDirection(const Vector3& direction) const;
        Vector3 transformVector(const Vector3& vector) const;
        Vector3 inverseTransformPoint(const Vector3& point) const;
        Vector3 inverseTransformDirection(const Vector3& direction) const;
        Vector3 inverseTransformVector(const Vector3& vector) const;

        static const Transform identity;

        Transform& operator=(const Transform& transform);
        Transform operator*(const Transform& transform) const;

    private:
        Vector3 position;
        Quaternion rotation;
        Vector3 scale;
};

#endif