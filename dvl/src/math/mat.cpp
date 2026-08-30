#include "dvl/math/mat.h"

#include <cmath>

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

    Mat4 Mat4::Perspective(float fovYRadians, float aspectRatio, float nearPlane, float farPlane)
    {
        float f = 1.0f / std::tan(fovYRadians / 2.0f);
        float nf = 1.0f / (nearPlane - farPlane);

        Mat4 result;

        result.m[0][0] = f / aspectRatio;
        result.m[1][1] = f;
        result.m[2][2] = (farPlane + nearPlane) * nf;
        result.m[2][3] = -1.0f;
        result.m[3][2] = (2.0f * farPlane * nearPlane) * nf;
        result.m[3][3] = 0.0f;

        return result;
    }

    Mat4 Mat4::Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
    {
        Mat4 result;

        result.m[0][0] = 2.0f / (right - left);
        result.m[1][1] = 2.0f / (top - bottom);
        result.m[2][2] = -2.0f / (farPlane - nearPlane);
        result.m[3][0] = -(right + left) / (right - left);
        result.m[3][1] = -(top + bottom) / (top - bottom);
        result.m[3][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);

        return result;
    }

    Mat4 Mat4::Inverse(const Mat4& matrix)
    {
        Mat4 result{};

        const float det = matrix.m[0][0] * (matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1]) -
                          matrix.m[0][1] * (matrix.m[1][0] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][0]) +
                          matrix.m[0][2] * (matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0]);

        if (std::abs(det) < 1e-6f)
            return Mat4::Identity();

        const float invDet = 1.0f / det;

        result.m[0][0] = (matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1]) * invDet;
        result.m[0][1] = (matrix.m[0][2] * matrix.m[2][1] - matrix.m[0][1] * matrix.m[2][2]) * invDet;
        result.m[0][2] = (matrix.m[0][1] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][1]) * invDet;

        result.m[1][0] = (matrix.m[1][2] * matrix.m[2][0] - matrix.m[1][0] * matrix.m[2][2]) * invDet;
        result.m[1][1] = (matrix.m[0][0] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][0]) * invDet;
        result.m[1][2] = (matrix.m[0][2] * matrix.m[1][0] - matrix.m[0][0] * matrix.m[1][2]) * invDet;

        result.m[2][0] = (matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0]) * invDet;
        result.m[2][1] = (matrix.m[0][1] * matrix.m[2][0] - matrix.m[0][0] * matrix.m[2][1]) * invDet;
        result.m[2][2] = (matrix.m[0][0] * matrix.m[1][1] - matrix.m[0][1] * matrix.m[1][0]) * invDet;

        result.m[0][3] = 0.0f;
        result.m[1][3] = 0.0f;
        result.m[2][3] = 0.0f;

        result.m[3][0] = -(matrix.m[3][0] * result.m[0][0] + matrix.m[3][1] * result.m[1][0] + matrix.m[3][2] * result.m[2][0]);

        result.m[3][1] = -(matrix.m[3][0] * result.m[0][1] + matrix.m[3][1] * result.m[1][1] + matrix.m[3][2] * result.m[2][1]);

        result.m[3][2] = -(matrix.m[3][0] * result.m[0][2] + matrix.m[3][1] * result.m[1][2] + matrix.m[3][2] * result.m[2][2]);

        result.m[3][3] = 1.0f;

        return result;
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
