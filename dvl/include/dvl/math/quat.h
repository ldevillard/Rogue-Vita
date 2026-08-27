#pragma once

#include "vec.h"

namespace dvl
{
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

        float x;
        float y;
        float z;
        float w;
    };

    float Dot(const Quat& a, const Quat& b);

    Quat Slerp(const Quat& a, const Quat& b, float t);
}