#include "dvl/math/mat.h"

#include "dvl/math/transform.h"

namespace dvl
{
    Mat4::Mat4()
        : m{}
    {
        m[0][0] = 1.0f;
        m[1][1] = 1.0f;
        m[2][2] = 1.0f;
        m[3][3] = 1.0f;
    }

    Mat4 Mat4::Identity()
    {
        return Mat4();
    }

    Mat4 Mat4::Translation(const Vec3& translation)
    {
        Mat4 result;
        result.m[3][0] = translation.x;
        result.m[3][1] = translation.y;
        result.m[3][2] = translation.z;
        return result;
    }

    Mat4 Mat4::Scale(const Vec3& scale)
    {
        Mat4 result;
        result.m[0][0] = scale.x;
        result.m[1][1] = scale.y;
        result.m[2][2] = scale.z;
        return result;
    }

    Mat4 Mat4::Rotation(const Quat& rotation)
    {
        const Quat q = rotation.Normalized();
        const float xx = q.x * q.x;
        const float yy = q.y * q.y;
        const float zz = q.z * q.z;
        const float xy = q.x * q.y;
        const float xz = q.x * q.z;
        const float yz = q.y * q.z;
        const float xw = q.x * q.w;
        const float yw = q.y * q.w;
        const float zw = q.z * q.w;

        Mat4 result;
        result.m[0][0] = 1.0f - 2.0f * (yy + zz);
        result.m[0][1] = 2.0f * (xy + zw);
        result.m[0][2] = 2.0f * (xz - yw);

        result.m[1][0] = 2.0f * (xy - zw);
        result.m[1][1] = 1.0f - 2.0f * (xx + zz);
        result.m[1][2] = 2.0f * (yz + xw);

        result.m[2][0] = 2.0f * (xz + yw);
        result.m[2][1] = 2.0f * (yz - xw);
        result.m[2][2] = 1.0f - 2.0f * (xx + yy);
        return result;
    }

    Mat4 Mat4::FromTransform(const Transform& transform)
    {
        return Translation(transform.translation.XYZ())
            * Rotation(transform.rotation)
            * Scale(transform.scale.XYZ());
    }

    Mat4 Mat4::operator*(const Mat4& rhs) const
    {
        Mat4 result;

        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                result.m[column][row] = 0.0f;

                for (int element = 0; element < 4; element++)
                    result.m[column][row] += m[element][row] * rhs.m[column][element];
            }
        }

        return result;
    }

    Vec4 Mat4::operator*(const Vec4& rhs) const
    {
        return Vec4(
            m[0][0] * rhs.x + m[1][0] * rhs.y + m[2][0] * rhs.z + m[3][0] * rhs.w,
            m[0][1] * rhs.x + m[1][1] * rhs.y + m[2][1] * rhs.z + m[3][1] * rhs.w,
            m[0][2] * rhs.x + m[1][2] * rhs.y + m[2][2] * rhs.z + m[3][2] * rhs.w,
            m[0][3] * rhs.x + m[1][3] * rhs.y + m[2][3] * rhs.z + m[3][3] * rhs.w);
    }
}
