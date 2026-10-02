#pragma once

namespace dvl
{
    class Vec2
    {
    public:
        Vec2();
        Vec2(float x, float y);

        static Vec2 Zero();
        static Vec2 One();

        Vec2 operator+(const Vec2& rhs) const;
        Vec2 operator-(const Vec2& rhs) const;
        Vec2 operator*(const Vec2& rhs) const;
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

        static Vec3 Zero();
        static Vec3 One();

        Vec3 operator+(const Vec3& rhs) const;
        Vec3 operator-(const Vec3& rhs) const;
        Vec3 operator*(const Vec3& rhs) const;
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

        static Vec4 Zero();
        static Vec4 One();

        Vec3 XYZ() const;

        Vec4 operator+(const Vec4& rhs) const;
        Vec4 operator-(const Vec4& rhs) const;
        Vec4 operator*(const Vec4& rhs) const;
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

    Vec2 Abs(const Vec2& v);
    Vec3 Abs(const Vec3& v);
    Vec4 Abs(const Vec4& v);

    float Dot(const Vec2& a, const Vec2& b);
    float Dot(const Vec3& a, const Vec3& b);
    float Dot(const Vec4& a, const Vec4& b);

    Vec2 Reflect(const Vec2& vec, const Vec2& normal);
    Vec3 Reflect(const Vec3& vec, const Vec3& normal);
    Vec4 Reflect(const Vec4& vec, const Vec4& normal);

    Vec3 Cross(const Vec3& a, const Vec3& b);

    Vec2 Lerp(const Vec2& a, const Vec2& b, float t);
    Vec3 Lerp(const Vec3& a, const Vec3& b, float t);
    Vec4 Lerp(const Vec4& a, const Vec4& b, float t);
}
