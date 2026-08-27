#pragma once

#include "quat.h"
#include "vec.h"

namespace dvl
{
    class Mat4
    {
    public:
        Mat4();

        static Mat4 Identity();

        static Mat4 Translation(const Vec3& translation);
        static Mat4 Scale(const Vec3& scale);
        static Mat4 Rotation(const Quat& rotation);

        Mat4 operator*(const Mat4& rhs) const;
        Vec4 operator*(const Vec4& rhs) const;

        // Column-major storage: m[column][row].
        float m[4][4];
    };
}
