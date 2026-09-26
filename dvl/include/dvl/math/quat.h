#pragma once

#include "vec.h"

namespace dvl
{
    class Mat4;

    class Quat
    {
    public:
        Quat();
        Quat(float x, float y, float z, float w);

        float Length() const;
        float LengthSquared() const;

        void Normalize();
        Quat Normalized() const;

        Quat operator*(const Quat& rhs) const;

        static Quat Identity();

        static Quat FromAxisAngle(const Vec3& axis, float angleRadians);
        static Quat FromMatrix(const Mat4& matrix);
        
        static Quat LookRotation(const Vec3& forward, const Vec3& up = {0.0f, 1.0f, 0.0f});

        float x;
        float y;
        float z;
        float w;
    };

    float Dot(const Quat& a, const Quat& b);

    Quat Nlerp(const Quat& a, const Quat& b, float t);
    Quat Slerp(const Quat& a, const Quat& b, float t);
}