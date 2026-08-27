#include "dvl/math/quat.h"

#include <cmath>

namespace dvl
{
    Quat::Quat()
        : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
    {
    }

    Quat::Quat(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w)
    {
    }

    float Quat::Length() const
    {
        return std::sqrt(LengthSquared());
    }

    float Quat::LengthSquared() const
    {
        return x * x + y * y + z * z + w * w;
    }

    void Quat::Normalize()
    {
        const float length = Length();
        if (length == 0.0f)
            return;

        x /= length;
        y /= length;
        z /= length;
        w /= length;
    }

    Quat Quat::Normalized() const
    {
        const float length = Length();
        if (length == 0.0f)
            return Quat(0.0f, 0.0f, 0.0f, 0.0f);

        return Quat(x / length, y / length, z / length, w / length);
    }

    Quat Quat::operator*(const Quat& rhs) const
    {
        return Quat(
            w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
            w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
            w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w,
            w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z);
    }

    Quat Quat::Identity()
    {
        return Quat();
    }

    Quat Quat::FromAxisAngle(const Vec3& axis, float angleRadians)
    {
        if (axis.LengthSquared() == 0.0f)
            return Identity();

        const Vec3 normalizedAxis = axis.Normalized();
        const float halfAngle = angleRadians * 0.5f;
        const float sinHalfAngle = std::sin(halfAngle);

        return Quat(
            normalizedAxis.x * sinHalfAngle,
            normalizedAxis.y * sinHalfAngle,
            normalizedAxis.z * sinHalfAngle,
            std::cos(halfAngle));
    }

    float Dot(const Quat& a, const Quat& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    Quat Slerp(const Quat& a, const Quat& b, float t)
    {
        Quat start = a.Normalized();
        Quat end = b.Normalized();

        float dot = Dot(start, end);
        if (dot < 0.0f)
        {
            end.x = -end.x;
            end.y = -end.y;
            end.z = -end.z;
            end.w = -end.w;
            dot = -dot;
        }

        if (dot > 1.0f)
            dot = 1.0f;

        constexpr float LinearThreshold = 0.9995f;
        if (dot > LinearThreshold)
        {
            const Quat result(
                start.x + (end.x - start.x) * t,
                start.y + (end.y - start.y) * t,
                start.z + (end.z - start.z) * t,
                start.w + (end.w - start.w) * t);
            return result.Normalized();
        }

        const float angle = std::acos(dot);
        const float sinAngle = std::sin(angle);
        const float startScale = std::sin((1.0f - t) * angle) / sinAngle;
        const float endScale = std::sin(t * angle) / sinAngle;

        return Quat(
            start.x * startScale + end.x * endScale,
            start.y * startScale + end.y * endScale,
            start.z * startScale + end.z * endScale,
            start.w * startScale + end.w * endScale);
    }
}
