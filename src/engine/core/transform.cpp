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
    return dvl::Mat4::Translation(position) * dvl::Mat4::Rotation(rotation) * dvl::Mat4::Scale(scale);
}

void Transform::LookAt(const dvl::Vec3& target)
{
    LookDirection(target - position);
}

void Transform::LookDirection(const dvl::Vec3& direction)
{
    rotation = dvl::Quat::LookRotation(direction);
}
