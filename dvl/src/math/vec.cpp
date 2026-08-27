#include "dvl/math/vec.h"

#include <cmath>

namespace dvl
{
    Vec3::Vec3()
        : x(0.0f), y(0.0f), z(0.0f)
    {
    }

    Vec3::Vec3(float x, float y, float z)
        : x(x), y(y), z(z)
    {
    }

    Vec3 Vec3::operator+(const Vec3& rhs) const
    {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vec3 Vec3::operator-(const Vec3& rhs) const
    {
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vec3 Vec3::operator*(float scalar) const
    {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    Vec3 Vec3::operator/(float scalar) const
    {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    Vec3& Vec3::operator+=(const Vec3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vec3& Vec3::operator-=(const Vec3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    Vec3& Vec3::operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    float Vec3::Length() const
    {
        return std::sqrt(LengthSquared());
    }

    float Vec3::LengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    Vec3 Vec3::Normalized() const
    {
        const float length = Length();
        if (length == 0.0f)
            return Vec3();

        return *this / length;
    }

    void Vec3::Normalize()
    {
        const float length = Length();
        if (length == 0.0f)
            return;

        x /= length;
        y /= length;
        z /= length;
    }

    Vec4::Vec4()
        : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
    {
    }

    Vec4::Vec4(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w)
    {
    }

    Vec4 Vec4::operator+(const Vec4& rhs) const
    {
        return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }

    Vec4 Vec4::operator-(const Vec4& rhs) const
    {
        return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }

    Vec4 Vec4::operator*(float scalar) const
    {
        return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    Vec4 Vec4::operator/(float scalar) const
    {
        return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
    }

    Vec4& Vec4::operator+=(const Vec4& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    Vec4& Vec4::operator-=(const Vec4& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    Vec4& Vec4::operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    float Vec4::Length() const
    {
        return std::sqrt(LengthSquared());
    }

    float Vec4::LengthSquared() const
    {
        return x * x + y * y + z * z + w * w;
    }

    Vec4 Vec4::Normalized() const
    {
        const float length = Length();
        if (length == 0.0f)
            return Vec4();

        return *this / length;
    }

    void Vec4::Normalize()
    {
        const float length = Length();
        if (length == 0.0f)
            return;

        x /= length;
        y /= length;
        z /= length;
        w /= length;
    }

    float Dot(const Vec3& a, const Vec3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    float Dot(const Vec4& a, const Vec4& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    Vec3 Cross(const Vec3& a, const Vec3& b)
    {
        return Vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x);
    }

    Vec3 Lerp(const Vec3& a, const Vec3& b, float t)
    {
        return a + (b - a) * t;
    }

    Vec4 Lerp(const Vec4& a, const Vec4& b, float t)
    {
        return a + (b - a) * t;
    }
}
