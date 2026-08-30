#include "../unit_test.h"

#include <cmath>

#include "dvl/math/mat.h"
#include "dvl/math/transform.h"

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

DVL_TEST(Mat4SubscriptOperatorAccessesColumnsThenRows)
{
    dvl::Mat4 matrix;
    matrix[2][1] = 42.0f;

    DVL_EXPECT_EQ(matrix.m[2][1], 42.0f);

    const dvl::Mat4& constMatrix = matrix;
    DVL_EXPECT_EQ(constMatrix[2][1], 42.0f);

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

DVL_TEST(Mat4FromDefaultTransformCreatesIdentity)
{
    const dvl::Mat4 matrix = dvl::Mat4::FromTransform(dvl::Transform());

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            const float expected = column == row ? 1.0f : 0.0f;
            DVL_EXPECT_NEAR(matrix.m[column][row], expected, Epsilon);
        }
    }

    return true;
}

DVL_TEST(Mat4FromTransformAppliesScaleRotationThenTranslation)
{
    dvl::Transform transform;
    transform.rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), Pi / 2.0f);
    transform.translation = dvl::Vec4(10.0f, 20.0f, 30.0f, 42.0f);
    transform.scale = dvl::Vec4(2.0f, 3.0f, 4.0f, -12.0f);

    const dvl::Mat4 matrix = dvl::Mat4::FromTransform(transform);
    const dvl::Mat4 expected =
        dvl::Mat4::Translation(dvl::Vec3(10.0f, 20.0f, 30.0f)) *
        dvl::Mat4::Rotation(transform.rotation) *
        dvl::Mat4::Scale(dvl::Vec3(2.0f, 3.0f, 4.0f));

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
            DVL_EXPECT_NEAR(matrix.m[column][row], expected.m[column][row], Epsilon);
    }

    const dvl::Vec4 transformedPoint = matrix * dvl::Vec4(1.0f, 2.0f, 3.0f, 1.0f);
    DVL_EXPECT_NEAR(transformedPoint.x, 4.0f, Epsilon);
    DVL_EXPECT_NEAR(transformedPoint.y, 22.0f, Epsilon);
    DVL_EXPECT_NEAR(transformedPoint.z, 42.0f, Epsilon);
    DVL_EXPECT_NEAR(transformedPoint.w, 1.0f, Epsilon);

    return true;
}

DVL_TEST(Mat4PerspectiveMapsFrustumBoundsToNormalizedDeviceCoordinates)
{
    constexpr float FovY = Pi / 2.0f;
    constexpr float AspectRatio = 2.0f;
    constexpr float NearPlane = 1.0f;
    constexpr float FarPlane = 11.0f;

    const dvl::Mat4 perspective = dvl::Mat4::Perspective(FovY, AspectRatio, NearPlane, FarPlane);

    const dvl::Vec4 nearCenterClip = perspective * dvl::Vec4(0.0f, 0.0f, -NearPlane, 1.0f);
    const dvl::Vec4 farCenterClip = perspective * dvl::Vec4(0.0f, 0.0f, -FarPlane, 1.0f);

    DVL_EXPECT_NEAR(nearCenterClip.z / nearCenterClip.w, -1.0f, Epsilon);
    DVL_EXPECT_NEAR(farCenterClip.z / farCenterClip.w, 1.0f, Epsilon);

    const float nearTop = NearPlane * std::tan(FovY / 2.0f);
    const float nearRight = nearTop * AspectRatio;
    const dvl::Vec4 nearTopRightClip = perspective * dvl::Vec4(nearRight, nearTop, -NearPlane, 1.0f);

    DVL_EXPECT_NEAR(nearTopRightClip.x / nearTopRightClip.w, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(nearTopRightClip.y / nearTopRightClip.w, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(nearTopRightClip.z / nearTopRightClip.w, -1.0f, Epsilon);

    return true;
}

DVL_TEST(Mat4OrthographicMapsBoundsToNormalizedDeviceCoordinates)
{
    constexpr float Left = -2.0f;
    constexpr float Right = 6.0f;
    constexpr float Bottom = -3.0f;
    constexpr float Top = 5.0f;
    constexpr float NearPlane = 1.0f;
    constexpr float FarPlane = 11.0f;

    const dvl::Mat4 orthographic = dvl::Mat4::Orthographic(
        Left, Right, Bottom, Top, NearPlane, FarPlane);

    const dvl::Vec4 nearBottomLeft = orthographic * dvl::Vec4(Left, Bottom, -NearPlane, 1.0f);
    DVL_EXPECT_NEAR(nearBottomLeft.x, -1.0f, Epsilon);
    DVL_EXPECT_NEAR(nearBottomLeft.y, -1.0f, Epsilon);
    DVL_EXPECT_NEAR(nearBottomLeft.z, -1.0f, Epsilon);
    DVL_EXPECT_NEAR(nearBottomLeft.w, 1.0f, Epsilon);

    const dvl::Vec4 farTopRight = orthographic * dvl::Vec4(Right, Top, -FarPlane, 1.0f);
    DVL_EXPECT_NEAR(farTopRight.x, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(farTopRight.y, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(farTopRight.z, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(farTopRight.w, 1.0f, Epsilon);

    const dvl::Vec4 center = orthographic * dvl::Vec4(
        (Left + Right) * 0.5f,
        (Bottom + Top) * 0.5f,
        -(NearPlane + FarPlane) * 0.5f,
        1.0f);
    DVL_EXPECT_NEAR(center.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(center.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(center.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(center.w, 1.0f, Epsilon);

    return true;
}

DVL_TEST(Mat4LookAtCreatesIdentityForTheCanonicalCamera)
{
    const dvl::Mat4 view = dvl::Mat4::LookAt(
        dvl::Vec3(0.0f, 0.0f, 0.0f),
        dvl::Vec3(0.0f, 0.0f, -1.0f),
        dvl::Vec3(0.0f, 1.0f, 0.0f));

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            const float expected = column == row ? 1.0f : 0.0f;
            DVL_EXPECT_NEAR(view.m[column][row], expected, Epsilon);
        }
    }

    return true;
}

DVL_TEST(Mat4LookAtTransformsEyeAndTargetIntoViewSpace)
{
    const dvl::Vec3 eye(2.0f, 3.0f, 4.0f);
    const dvl::Vec3 target(-1.0f, 1.0f, -2.0f);
    const dvl::Mat4 view = dvl::Mat4::LookAt(eye, target, dvl::Vec3(0.0f, 1.0f, 0.0f));

    const dvl::Vec4 viewEye = view * dvl::Vec4(eye.x, eye.y, eye.z, 1.0f);
    DVL_EXPECT_NEAR(viewEye.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(viewEye.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(viewEye.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(viewEye.w, 1.0f, Epsilon);

    const dvl::Vec4 viewTarget = view * dvl::Vec4(target.x, target.y, target.z, 1.0f);
    DVL_EXPECT_NEAR(viewTarget.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(viewTarget.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(viewTarget.z, -(target - eye).Length(), Epsilon);
    DVL_EXPECT_NEAR(viewTarget.w, 1.0f, Epsilon);

    const dvl::Mat4 world = dvl::Mat4::Inverse(view);
    DVL_EXPECT_NEAR(world.m[3][0], eye.x, Epsilon);
    DVL_EXPECT_NEAR(world.m[3][1], eye.y, Epsilon);
    DVL_EXPECT_NEAR(world.m[3][2], eye.z, Epsilon);

    const dvl::Vec3 expectedForward = (target - eye).Normalized();
    DVL_EXPECT_NEAR(-world.m[2][0], expectedForward.x, Epsilon);
    DVL_EXPECT_NEAR(-world.m[2][1], expectedForward.y, Epsilon);
    DVL_EXPECT_NEAR(-world.m[2][2], expectedForward.z, Epsilon);

    return true;
}

DVL_TEST(Mat4InverseOfIdentityIsIdentity)
{
    const dvl::Mat4 inverse = dvl::Mat4::Inverse(dvl::Mat4::Identity());

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            const float expected = column == row ? 1.0f : 0.0f;
            DVL_EXPECT_NEAR(inverse.m[column][row], expected, Epsilon);
        }
    }

    return true;
}

DVL_TEST(Mat4InverseOfAffineTransformComposesToIdentityAndRestoresVectors)
{
    const dvl::Mat4 transform =
        dvl::Mat4::Translation(dvl::Vec3(3.0f, -4.0f, 5.0f)) *
        dvl::Mat4::Rotation(dvl::Quat::FromAxisAngle(dvl::Vec3(1.0f, 2.0f, -3.0f), Pi / 3.0f)) *
        dvl::Mat4::Scale(dvl::Vec3(2.0f, 0.5f, -1.5f));
    const dvl::Mat4 inverse = dvl::Mat4::Inverse(transform);

    const dvl::Mat4 leftIdentity = transform * inverse;
    const dvl::Mat4 rightIdentity = inverse * transform;

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            const float expected = column == row ? 1.0f : 0.0f;
            DVL_EXPECT_NEAR(leftIdentity.m[column][row], expected, Epsilon);
            DVL_EXPECT_NEAR(rightIdentity.m[column][row], expected, Epsilon);
        }
    }

    const dvl::Vec4 point(1.5f, -2.0f, 4.0f, 1.0f);
    const dvl::Vec4 restoredPoint = inverse * (transform * point);
    DVL_EXPECT_NEAR(restoredPoint.x, point.x, Epsilon);
    DVL_EXPECT_NEAR(restoredPoint.y, point.y, Epsilon);
    DVL_EXPECT_NEAR(restoredPoint.z, point.z, Epsilon);
    DVL_EXPECT_NEAR(restoredPoint.w, point.w, Epsilon);

    const dvl::Vec4 direction(-1.0f, 3.0f, 2.0f, 0.0f);
    const dvl::Vec4 restoredDirection = inverse * (transform * direction);
    DVL_EXPECT_NEAR(restoredDirection.x, direction.x, Epsilon);
    DVL_EXPECT_NEAR(restoredDirection.y, direction.y, Epsilon);
    DVL_EXPECT_NEAR(restoredDirection.z, direction.z, Epsilon);
    DVL_EXPECT_NEAR(restoredDirection.w, direction.w, Epsilon);

    return true;
}

DVL_TEST(Mat4InverseOfSingularAffineMatrixReturnsIdentity)
{
    const dvl::Mat4 singular = dvl::Mat4::Scale(dvl::Vec3(2.0f, 0.0f, -3.0f));
    const dvl::Mat4 inverse = dvl::Mat4::Inverse(singular);

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            const float expected = column == row ? 1.0f : 0.0f;
            DVL_EXPECT_NEAR(inverse.m[column][row], expected, Epsilon);
        }
    }

    return true;
}
