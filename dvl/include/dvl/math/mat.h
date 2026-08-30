#pragma once

#include "quat.h"
#include "vec.h"

namespace dvl
{
    struct Transform;

    class Mat4
    {
    public:
        Mat4();

        static Mat4 Identity();

        static Mat4 Translation(const Vec3& translation);
        static Mat4 Scale(const Vec3& scale);
        static Mat4 Rotation(const Quat& rotation);

        static Mat4 FromTransform(const Transform& transform);

        static Mat4 Perspective(float fovYRadians, float aspectRatio, float nearPlane, float farPlane);
        static Mat4 Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
        static Mat4 Inverse(const Mat4& matrix);

        Mat4 operator*(const Mat4& rhs) const;
        Vec4 operator*(const Vec4& rhs) const;

        // Column-major storage: m[column][row].
        float m[4][4];
    };
}
