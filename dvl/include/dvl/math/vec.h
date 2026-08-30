#pragma once

namespace dvl
{
    class Vec2
    {
    public:
        Vec2();
        Vec2(float x, float y);

        Vec2 operator+(const Vec2& rhs) const;
        Vec2 operator-(const Vec2& rhs) const;
        Vec2 operator*(float scalar) const;
        Vec2 operator/(float scalar) const;

        Vec2& operator+=(const Vec2& rhs);
        Vec2& operator-=(const Vec2& rhs);
        Vec2& operator*=(float scalar);

        float Length() const;
        float LengthSquared() const;

        Vec2 Normalized() const;
        void Normalize();

        float x;
        float y;
    };

    class Vec3
    {
    public:
        Vec3();
        Vec3(float x, float y, float z);

        Vec3 operator+(const Vec3& rhs) const;
        Vec3 operator-(const Vec3& rhs) const;
        Vec3 operator*(float scalar) const;
        Vec3 operator/(float scalar) const;

        Vec3& operator+=(const Vec3& rhs);
        Vec3& operator-=(const Vec3& rhs);
        Vec3& operator*=(float scalar);

        float Length() const;
        float LengthSquared() const;

        Vec3 Normalized() const;
        void Normalize();

        float x;
        float y;
        float z;
    };

    struct Vec4
    {
        Vec4();
        Vec4(float x, float y, float z, float w);

        Vec3 XYZ() const;

        Vec4 operator+(const Vec4& rhs) const;
        Vec4 operator-(const Vec4& rhs) const;
        Vec4 operator*(float scalar) const;
        Vec4 operator/(float scalar) const;

        Vec4& operator+=(const Vec4& rhs);
        Vec4& operator-=(const Vec4& rhs);
        Vec4& operator*=(float scalar);

        float Length() const;
        float LengthSquared() const;

        Vec4 Normalized() const;
        void Normalize();

        float x;
        float y;
        float z;
        float w;
    };

    float Dot(const Vec2& a, const Vec2& b);
    float Dot(const Vec3& a, const Vec3& b);
    float Dot(const Vec4& a, const Vec4& b);

    Vec3 Cross(const Vec3& a, const Vec3& b);

    Vec2 Lerp(const Vec2& a, const Vec2& b, float t);
    Vec3 Lerp(const Vec3& a, const Vec3& b, float t);
    Vec4 Lerp(const Vec4& a, const Vec4& b, float t);
}
