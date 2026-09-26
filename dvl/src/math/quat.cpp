#include "dvl/math/quat.h"

#include <algorithm>
#include <cmath>

#include "dvl/math/mat.h"

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
        return Quat(w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
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

        return Quat(normalizedAxis.x * sinHalfAngle,
                    normalizedAxis.y * sinHalfAngle,
                    normalizedAxis.z * sinHalfAngle,
                    std::cos(halfAngle));
    }

    Quat Quat::FromMatrix(const Mat4& matrix)
    {
        Quat q;

        q.w = 0.5f * std::sqrt(std::max(0.0f, 1.0f + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2]));
        q.x = 0.5f * std::sqrt(std::max(0.0f, 1.0f + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2]));
        q.y = 0.5f * std::sqrt(std::max(0.0f, 1.0f - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2]));
        q.z = 0.5f * std::sqrt(std::max(0.0f, 1.0f - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2]));

        q.x = std::copysign(q.x, matrix.m[1][2] - matrix.m[2][1]);
        q.y = std::copysign(q.y, matrix.m[2][0] - matrix.m[0][2]);
        q.z = std::copysign(q.z, matrix.m[0][1] - matrix.m[1][0]);

        return q.Normalized();
    }

    Quat Quat::LookRotation(const Vec3& forward, const Vec3& up)
    {
        return FromMatrix(Mat4::LookRotation(forward, up));
    }

    float Dot(const Quat& a, const Quat& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    Quat Nlerp(const Quat& a, const Quat& b, float t)
    {
        // q and -q encode the same rotation. Flip the second quaternion so the linear interpolation follows the shortest arc
        const float endSign = Dot(a, b) < 0.0f ? -1.0f : 1.0f;

        const Quat result(a.x + (b.x * endSign - a.x) * t,
                          a.y + (b.y * endSign - a.y) * t,
                          a.z + (b.z * endSign - a.z) * t,
                          a.w + (b.w * endSign - a.w) * t);

        return result.Normalized();
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
            const Quat result(start.x + (end.x - start.x) * t,
                              start.y + (end.y - start.y) * t,
                              start.z + (end.z - start.z) * t,
                              start.w + (end.w - start.w) * t);
            return result.Normalized();
        }

        const float angle = std::acos(dot);
        const float sinAngle = std::sin(angle);
        const float startScale = std::sin((1.0f - t) * angle) / sinAngle;
        const float endScale = std::sin(t * angle) / sinAngle;

        return Quat(start.x * startScale + end.x * endScale,
                    start.y * startScale + end.y * endScale,
                    start.z * startScale + end.z * endScale,
                    start.w * startScale + end.w * endScale);
    }
}
