#include "Transform.h"

const Transform Transform::identity = Transform(Vector3(0, 0, 0), Quaternion(1, 0, 0, 0), Vector3(1, 1, 1));

Transform::Transform()
{
    *this = identity;
}

Transform::Transform(const Vector3& translation, const Quaternion& rotation)
{
    this->position = translation;
    this->rotation = rotation;
    this->scale = Vector3(1, 1, 1);
}

Transform::Transform(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
{
    this->position = translation;
    this->rotation = rotation;
    this->scale = scale;
}

Transform::Transform(const Transform& transform)
{
    this->position = transform.position;
    this->rotation = transform.rotation;
    this->scale = transform.scale;
}

Transform::~Transform()
{
}

Vector3 Transform::getPosition() const
{
    return position;
}

Vector3 Transform::getScale() const
{
    return scale;
}

Vector3 Transform::getEulerAngles() const
{
    return rotation.getEulerAngles();
}

Quaternion Transform::getRotation() const
{
    return rotation;
}

void Transform::setPosition(const Vector3& pos)
{
    position = pos;
}

void Transform::setScale(const Vector3& scale)
{
    this->scale = scale;
}

void Transform::setRotation(const Quaternion& rot)
{
    rotation = rot;
}

void Transform::setRotation(const Vector3& eulerAngles)
{
    rotation.setEulerAngles(eulerAngles);
}

void Transform::translate(const Vector3& translation)
{
    position += translation;
}

void Transform::rotate(const Quaternion& rotation)
{
    this->rotation *= rotation;
}

void Transform::rotate(const Vector3& eulerAngles)
{
    this->rotation *= Quaternion(eulerAngles);
}

void Transform::dialate(const Vector3& scale)
{
    this->scale.x *= scale.x;
    this->scale.y *= scale.y;
    this->scale.z *= scale.z;
}

Vector3 Transform::transformPoint(const Vector3& point) const
{
    // transform using SRT
    Vector3 result = point;
    result.x *= scale.x;
    result.y *= scale.y;
    result.z *= scale.z;

    result = rotation * result;
    result += position;

    return result;
}

Vector3 Transform::transformDirection(const Vector3& direction) const
{
    return rotation * direction;
}

Vector3 Transform::transformVector(const Vector3& vector) const
{
    Vector3 result = vector;
    result.x *= scale.x;
    result.y *= scale.y;
    result.z *= scale.z;

    result = rotation * result;

    return result;
}

Vector3 Transform::inverseTransformPoint(const Vector3& point) const
{
    Vector3 result = point - position;
    result = rotation.inverse() * result;
    result.x /= scale.x;
    result.y /= scale.y;
    result.z /= scale.z;

    return result;
}

Vector3 Transform::inverseTransformDirection(const Vector3& direction) const
{
    return rotation.inverse() * direction;
}

Vector3 Transform::inverseTransformVector(const Vector3& vector) const
{
    Vector3 result = vector;
    result = rotation.inverse() * result;
    result.x /= scale.x;
    result.y /= scale.y;
    result.z /= scale.z;

    return result;
}

Transform& Transform::operator=(const Transform& transform)
{
    this->position = transform.position;
    this->rotation = transform.rotation;
    this->scale = transform.scale;

    return *this;
}

Transform Transform::operator*(const Transform& transform) const
{
    Transform result;
    result.position = transformPoint(transform.position);
    result.rotation = rotation * transform.rotation;
    
    Vector3 newScale;
    newScale.x = scale.x * transform.scale.x;
    newScale.y = scale.y * transform.scale.y;
    newScale.z = scale.z * transform.scale.z;

    result.scale = newScale;

    return result;
}

