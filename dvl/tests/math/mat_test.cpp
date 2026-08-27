#include "../unit_test.h"

#include "dvl/math/mat.h"

namespace
{
    constexpr float Epsilon = 0.0001f;
    constexpr float Pi = 3.14159265358979323846f;
}

DVL_TEST(Mat4DefaultConstructorAndIdentityCreateIdentityMatrix)
{
    const dvl::Mat4 defaultMatrix;
    const dvl::Mat4 identity = dvl::Mat4::Identity();

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            const float expected = column == row ? 1.0f : 0.0f;
            DVL_EXPECT_EQ(defaultMatrix.m[column][row], expected);
            DVL_EXPECT_EQ(identity.m[column][row], expected);
        }
    }

    const dvl::Vec4 value(1.0f, -2.0f, 3.0f, 1.0f);
    const dvl::Vec4 unchanged = identity * value;
    DVL_EXPECT_EQ(unchanged.x, value.x);
    DVL_EXPECT_EQ(unchanged.y, value.y);
    DVL_EXPECT_EQ(unchanged.z, value.z);
    DVL_EXPECT_EQ(unchanged.w, value.w);

    return true;
}

DVL_TEST(Mat4TranslationUsesHomogeneousCoordinatesAndColumnMajorStorage)
{
    const dvl::Mat4 translation = dvl::Mat4::Translation(dvl::Vec3(3.0f, -2.0f, 5.0f));

    DVL_EXPECT_EQ(translation.m[3][0], 3.0f);
    DVL_EXPECT_EQ(translation.m[3][1], -2.0f);
    DVL_EXPECT_EQ(translation.m[3][2], 5.0f);
    DVL_EXPECT_EQ(translation.m[0][3], 0.0f);
    DVL_EXPECT_EQ(translation.m[1][3], 0.0f);
    DVL_EXPECT_EQ(translation.m[2][3], 0.0f);

    const dvl::Vec4 translatedPoint = translation * dvl::Vec4(1.0f, 2.0f, 3.0f, 1.0f);
    DVL_EXPECT_EQ(translatedPoint.x, 4.0f);
    DVL_EXPECT_EQ(translatedPoint.y, 0.0f);
    DVL_EXPECT_EQ(translatedPoint.z, 8.0f);
    DVL_EXPECT_EQ(translatedPoint.w, 1.0f);

    const dvl::Vec4 unchangedDirection = translation * dvl::Vec4(1.0f, 2.0f, 3.0f, 0.0f);
    DVL_EXPECT_EQ(unchangedDirection.x, 1.0f);
    DVL_EXPECT_EQ(unchangedDirection.y, 2.0f);
    DVL_EXPECT_EQ(unchangedDirection.z, 3.0f);
    DVL_EXPECT_EQ(unchangedDirection.w, 0.0f);

    return true;
}

DVL_TEST(Mat4ScaleScalesEachAxisAndPreservesHomogeneousComponent)
{
    const dvl::Mat4 scale = dvl::Mat4::Scale(dvl::Vec3(2.0f, -3.0f, 0.5f));
    const dvl::Vec4 result = scale * dvl::Vec4(4.0f, 2.0f, -6.0f, 1.0f);

    DVL_EXPECT_EQ(result.x, 8.0f);
    DVL_EXPECT_EQ(result.y, -6.0f);
    DVL_EXPECT_EQ(result.z, -3.0f);
    DVL_EXPECT_EQ(result.w, 1.0f);

    return true;
}

DVL_TEST(Mat4RotationFromQuatRotatesVectorsAndPreservesTheirLength)
{
    const dvl::Quat quarterTurn = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 2.0f), Pi / 2.0f);
    const dvl::Mat4 rotation = dvl::Mat4::Rotation(quarterTurn);
    const dvl::Vec4 rotated = rotation * dvl::Vec4(2.0f, 0.0f, 0.0f, 0.0f);

    DVL_EXPECT_NEAR(rotated.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(rotated.y, 2.0f, Epsilon);
    DVL_EXPECT_NEAR(rotated.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(rotated.w, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Vec3(rotated.x, rotated.y, rotated.z).Length(), 2.0f, Epsilon);

    DVL_EXPECT_NEAR(rotation.m[0][0], 0.0f, Epsilon);
    DVL_EXPECT_NEAR(rotation.m[0][1], 1.0f, Epsilon);
    DVL_EXPECT_NEAR(rotation.m[1][0], -1.0f, Epsilon);
    DVL_EXPECT_NEAR(rotation.m[1][1], 0.0f, Epsilon);

    return true;
}

DVL_TEST(Mat4RotationNormalizesItsQuaternionInput)
{
    const dvl::Quat unitRotation = dvl::Quat::FromAxisAngle(dvl::Vec3(1.0f, -2.0f, 3.0f), Pi / 3.0f);
    const dvl::Quat scaledRotation(
        unitRotation.x * 4.0f,
        unitRotation.y * 4.0f,
        unitRotation.z * 4.0f,
        unitRotation.w * 4.0f);
    const dvl::Mat4 fromUnit = dvl::Mat4::Rotation(unitRotation);
    const dvl::Mat4 fromScaled = dvl::Mat4::Rotation(scaledRotation);

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
            DVL_EXPECT_NEAR(fromScaled.m[column][row], fromUnit.m[column][row], Epsilon);
    }

    const dvl::Mat4 fromZero = dvl::Mat4::Rotation(dvl::Quat(0.0f, 0.0f, 0.0f, 0.0f));
    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            const float expected = column == row ? 1.0f : 0.0f;
            DVL_EXPECT_EQ(fromZero.m[column][row], expected);
        }
    }

    return true;
}

DVL_TEST(Mat4MultiplicationComposesTransformsRightToLeft)
{
    const dvl::Mat4 translation = dvl::Mat4::Translation(dvl::Vec3(10.0f, 20.0f, 30.0f));
    const dvl::Mat4 scale = dvl::Mat4::Scale(dvl::Vec3(2.0f, 3.0f, 4.0f));
    const dvl::Vec4 point(1.0f, 2.0f, 3.0f, 1.0f);

    const dvl::Vec4 composed = (translation * scale) * point;
    const dvl::Vec4 sequential = translation * (scale * point);
    DVL_EXPECT_EQ(composed.x, 12.0f);
    DVL_EXPECT_EQ(composed.y, 26.0f);
    DVL_EXPECT_EQ(composed.z, 42.0f);
    DVL_EXPECT_EQ(composed.w, 1.0f);
    DVL_EXPECT_EQ(composed.x, sequential.x);
    DVL_EXPECT_EQ(composed.y, sequential.y);
    DVL_EXPECT_EQ(composed.z, sequential.z);
    DVL_EXPECT_EQ(composed.w, sequential.w);

    const dvl::Vec4 reverseOrder = (scale * translation) * point;
    DVL_EXPECT_EQ(reverseOrder.x, 22.0f);
    DVL_EXPECT_EQ(reverseOrder.y, 66.0f);
    DVL_EXPECT_EQ(reverseOrder.z, 132.0f);
    DVL_EXPECT_EQ(reverseOrder.w, 1.0f);

    return true;
}

DVL_TEST(Mat4MultiplicationIsAssociativeAndHasIdentity)
{
    const dvl::Mat4 translation = dvl::Mat4::Translation(dvl::Vec3(2.0f, -3.0f, 4.0f));
    const dvl::Mat4 rotation = dvl::Mat4::Rotation(
        dvl::Quat::FromAxisAngle(dvl::Vec3(1.0f, 2.0f, -1.0f), Pi / 4.0f));
    const dvl::Mat4 scale = dvl::Mat4::Scale(dvl::Vec3(2.0f, 3.0f, 0.5f));

    const dvl::Mat4 leftAssociated = (translation * rotation) * scale;
    const dvl::Mat4 rightAssociated = translation * (rotation * scale);
    const dvl::Mat4 leftIdentity = dvl::Mat4::Identity() * leftAssociated;
    const dvl::Mat4 rightIdentity = leftAssociated * dvl::Mat4::Identity();

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            DVL_EXPECT_NEAR(leftAssociated.m[column][row], rightAssociated.m[column][row], Epsilon);
            DVL_EXPECT_NEAR(leftIdentity.m[column][row], leftAssociated.m[column][row], Epsilon);
            DVL_EXPECT_NEAR(rightIdentity.m[column][row], leftAssociated.m[column][row], Epsilon);
        }
    }

    return true;
}
