#include "engine/core/transform.h"

#include <cmath>

dvl::Vec3 Transform::GetForward() const
{
    const dvl::Mat4 matrix = GetMatrix();
    return dvl::Vec3(-matrix[2][0], -matrix[2][1], -matrix[2][2]).Normalized();
}

dvl::Vec3 Transform::GetRight() const
{
    const dvl::Mat4 matrix = GetMatrix();
    return dvl::Vec3(matrix[0][0], matrix[0][1], matrix[0][2]).Normalized();
}

dvl::Vec3 Transform::GetUp() const
{
    const dvl::Mat4 matrix = GetMatrix();
    return dvl::Vec3(matrix[1][0], matrix[1][1], matrix[1][2]).Normalized();
}

dvl::Mat4 Transform::GetMatrix() const
{
    return dvl::Mat4::Translation(position) *
        dvl::Mat4::Rotation(dvl::Quat::FromAxisAngle(dvl::Vec3(1.0f, 0.0f, 0.0f), rotation.x)) *
        dvl::Mat4::Rotation(dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 1.0f, 0.0f), rotation.y)) *
        dvl::Mat4::Rotation(dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), rotation.z)) *
        dvl::Mat4::Scale(scale);
}

void Transform::LookAt(const dvl::Vec3& target)
{
    const dvl::Vec3 delta = target - position;

    if (delta.LengthSquared() < 0.000001f)
        return;

    const dvl::Mat4 worldMatrix = dvl::Mat4::Inverse(dvl::Mat4::LookAt(position, target, dvl::Vec3(0.0f, 1.0f, 0.0f)));

    rotation.y = std::asin(dvl::Clamp(worldMatrix[2][0], -1.0f, 1.0f));
    rotation.x = std::atan2(-worldMatrix[2][1], worldMatrix[2][2]);
    rotation.z = std::atan2(-worldMatrix[1][0], worldMatrix[0][0]);
}

void Transform::LookDirection(const dvl::Vec3& direction)
{
    const float lengthSquared = direction.LengthSquared();

    if (lengthSquared < 0.000001f)
        return;

    const dvl::Vec3 forward = direction * dvl::InverseSqrt(lengthSquared);

    rotation.x = 0.0f;
    rotation.y = std::atan2(-forward.x, -forward.z);
    rotation.z = 0.0f;
}
