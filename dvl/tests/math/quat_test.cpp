#include "../unit_test.h"

#include "dvl/math/quat.h"
#include "dvl/math/mat.h"

namespace
{
    constexpr float Epsilon = 0.0001f;
    constexpr float Pi = 3.14159265358979323846f;
}

DVL_TEST(QuatConstructorsAndIdentityInitializeComponents)
{
    const dvl::Quat defaultValue;
    DVL_EXPECT_EQ(defaultValue.x, 0.0f);
    DVL_EXPECT_EQ(defaultValue.y, 0.0f);
    DVL_EXPECT_EQ(defaultValue.z, 0.0f);
    DVL_EXPECT_EQ(defaultValue.w, 1.0f);

    const dvl::Quat value(1.0f, -2.0f, 3.0f, -4.0f);
    DVL_EXPECT_EQ(value.x, 1.0f);
    DVL_EXPECT_EQ(value.y, -2.0f);
    DVL_EXPECT_EQ(value.z, 3.0f);
    DVL_EXPECT_EQ(value.w, -4.0f);

    const dvl::Quat identity = dvl::Quat::Identity();
    DVL_EXPECT_EQ(identity.x, 0.0f);
    DVL_EXPECT_EQ(identity.y, 0.0f);
    DVL_EXPECT_EQ(identity.z, 0.0f);
    DVL_EXPECT_EQ(identity.w, 1.0f);

    return true;
}

DVL_TEST(QuatLengthDotAndNormalizationAreCorrect)
{
    const dvl::Quat value(1.0f, 2.0f, 2.0f, 0.0f);
    DVL_EXPECT_EQ(value.LengthSquared(), 9.0f);
    DVL_EXPECT_NEAR(value.Length(), 3.0f, Epsilon);
    DVL_EXPECT_EQ(dvl::Dot(value, dvl::Quat(2.0f, 3.0f, 4.0f, 5.0f)), 16.0f);

    const dvl::Quat normalized = value.Normalized();
    DVL_EXPECT_NEAR(normalized.x, 1.0f / 3.0f, Epsilon);
    DVL_EXPECT_NEAR(normalized.y, 2.0f / 3.0f, Epsilon);
    DVL_EXPECT_NEAR(normalized.z, 2.0f / 3.0f, Epsilon);
    DVL_EXPECT_NEAR(normalized.w, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(normalized.Length(), 1.0f, Epsilon);

    dvl::Quat mutableValue(0.0f, 0.0f, 0.0f, 2.0f);
    mutableValue.Normalize();
    DVL_EXPECT_NEAR(mutableValue.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(mutableValue.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(mutableValue.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(mutableValue.w, 1.0f, Epsilon);

    const dvl::Quat zero(0.0f, 0.0f, 0.0f, 0.0f);
    const dvl::Quat normalizedZero = zero.Normalized();
    DVL_EXPECT_EQ(normalizedZero.x, 0.0f);
    DVL_EXPECT_EQ(normalizedZero.y, 0.0f);
    DVL_EXPECT_EQ(normalizedZero.z, 0.0f);
    DVL_EXPECT_EQ(normalizedZero.w, 0.0f);

    return true;
}

DVL_TEST(QuatMultiplicationUsesHamiltonProduct)
{
    const dvl::Quat identity = dvl::Quat::Identity();
    const dvl::Quat value(1.0f, 2.0f, 3.0f, 4.0f);

    const dvl::Quat leftIdentity = identity * value;
    DVL_EXPECT_EQ(leftIdentity.x, value.x);
    DVL_EXPECT_EQ(leftIdentity.y, value.y);
    DVL_EXPECT_EQ(leftIdentity.z, value.z);
    DVL_EXPECT_EQ(leftIdentity.w, value.w);

    const dvl::Quat left(1.0f, 2.0f, 3.0f, 4.0f);
    const dvl::Quat right(5.0f, 6.0f, 7.0f, 8.0f);
    const dvl::Quat product = left * right;
    DVL_EXPECT_EQ(product.x, 24.0f);
    DVL_EXPECT_EQ(product.y, 48.0f);
    DVL_EXPECT_EQ(product.z, 48.0f);
    DVL_EXPECT_EQ(product.w, -6.0f);

    return true;
}

DVL_TEST(QuatFromAxisAngleNormalizesTheAxis)
{
    const dvl::Quat rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 2.0f, 0.0f), Pi);
    DVL_EXPECT_NEAR(rotation.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(rotation.y, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(rotation.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(rotation.w, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(rotation.Length(), 1.0f, Epsilon);

    const dvl::Quat zeroAxis = dvl::Quat::FromAxisAngle(dvl::Vec3(), Pi);
    DVL_EXPECT_EQ(zeroAxis.x, 0.0f);
    DVL_EXPECT_EQ(zeroAxis.y, 0.0f);
    DVL_EXPECT_EQ(zeroAxis.z, 0.0f);
    DVL_EXPECT_EQ(zeroAxis.w, 1.0f);

    return true;
}

DVL_TEST(QuatSlerpInterpolatesNormalizedShortestPath)
{
    const dvl::Quat identity = dvl::Quat::Identity();
    const dvl::Quat halfTurn = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), Pi);
    const dvl::Quat midpoint = dvl::Slerp(identity, halfTurn, 0.5f);

    DVL_EXPECT_NEAR(midpoint.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(midpoint.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(std::fabs(midpoint.z), std::sqrt(0.5f), Epsilon);
    DVL_EXPECT_NEAR(midpoint.w, std::sqrt(0.5f), Epsilon);
    DVL_EXPECT_NEAR(midpoint.Length(), 1.0f, Epsilon);

    const dvl::Quat negatedIdentity(0.0f, 0.0f, 0.0f, -1.0f);
    const dvl::Quat shortestPath = dvl::Slerp(identity, negatedIdentity, 0.5f);
    DVL_EXPECT_NEAR(shortestPath.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(shortestPath.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(shortestPath.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(shortestPath.w, 1.0f, Epsilon);

    return true;
}

DVL_TEST(QuatNlerpInterpolatesNormalizedShortestPath)
{
    const dvl::Quat identity = dvl::Quat::Identity();
    const dvl::Quat quarterTurn = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), Pi / 2.0f);
    const dvl::Quat midpoint = dvl::Nlerp(identity, quarterTurn, 0.5f);

    DVL_EXPECT_NEAR(midpoint.z, std::sin(Pi / 8.0f), Epsilon);
    DVL_EXPECT_NEAR(midpoint.w, std::cos(Pi / 8.0f), Epsilon);
    DVL_EXPECT_NEAR(midpoint.Length(), 1.0f, Epsilon);

    const dvl::Quat negatedIdentity(0.0f, 0.0f, 0.0f, -1.0f);
    const dvl::Quat shortestPath = dvl::Nlerp(identity, negatedIdentity, 0.5f);
    DVL_EXPECT_NEAR(shortestPath.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(shortestPath.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(shortestPath.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(shortestPath.w, 1.0f, Epsilon);

    return true;
}

DVL_TEST(QuatMultiplicationIsAssociativeAndPreservesRotationLength)
{
    const dvl::Quat xRotation = dvl::Quat::FromAxisAngle(dvl::Vec3(1.0f, 0.0f, 0.0f), Pi / 3.0f);
    const dvl::Quat yRotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 1.0f, 0.0f), Pi / 4.0f);
    const dvl::Quat arbitraryRotation = dvl::Quat::FromAxisAngle(dvl::Vec3(1.0f, -2.0f, 3.0f), Pi / 5.0f);

    const dvl::Quat leftAssociated = (xRotation * yRotation) * arbitraryRotation;
    const dvl::Quat rightAssociated = xRotation * (yRotation * arbitraryRotation);
    DVL_EXPECT_NEAR(leftAssociated.x, rightAssociated.x, Epsilon);
    DVL_EXPECT_NEAR(leftAssociated.y, rightAssociated.y, Epsilon);
    DVL_EXPECT_NEAR(leftAssociated.z, rightAssociated.z, Epsilon);
    DVL_EXPECT_NEAR(leftAssociated.w, rightAssociated.w, Epsilon);
    DVL_EXPECT_NEAR(leftAssociated.Length(), 1.0f, Epsilon);

    const dvl::Quat xy = xRotation * yRotation;
    const dvl::Quat yx = yRotation * xRotation;
    DVL_EXPECT_TRUE(std::fabs(xy.z - yx.z) > Epsilon);

    const dvl::Quat a(1.0f, -2.0f, 3.0f, 4.0f);
    const dvl::Quat b(-2.0f, 5.0f, 1.0f, 3.0f);
    DVL_EXPECT_NEAR((a * b).Length(), a.Length() * b.Length(), Epsilon);

    return true;
}

DVL_TEST(QuatSlerpHasExpectedEndpointsAndConstantAngularProgress)
{
    const dvl::Quat start = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 2.0f), Pi / 6.0f);
    const dvl::Quat end = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 3.0f), 5.0f * Pi / 6.0f);

    const dvl::Quat atStart = dvl::Slerp(start, end, 0.0f);
    const dvl::Quat atEnd = dvl::Slerp(start, end, 1.0f);
    DVL_EXPECT_NEAR(atStart.z, start.z, Epsilon);
    DVL_EXPECT_NEAR(atStart.w, start.w, Epsilon);
    DVL_EXPECT_NEAR(atEnd.z, end.z, Epsilon);
    DVL_EXPECT_NEAR(atEnd.w, end.w, Epsilon);

    const dvl::Quat firstQuarter = dvl::Slerp(start, end, 0.25f);
    const dvl::Quat midpoint = dvl::Slerp(start, end, 0.5f);
    const dvl::Quat thirdQuarter = dvl::Slerp(start, end, 0.75f);
    const dvl::Quat expectedFirstQuarter = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), Pi / 3.0f);
    const dvl::Quat expectedMidpoint = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), Pi / 2.0f);
    const dvl::Quat expectedThirdQuarter = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), 2.0f * Pi / 3.0f);

    DVL_EXPECT_NEAR(firstQuarter.z, expectedFirstQuarter.z, Epsilon);
    DVL_EXPECT_NEAR(firstQuarter.w, expectedFirstQuarter.w, Epsilon);
    DVL_EXPECT_NEAR(midpoint.z, expectedMidpoint.z, Epsilon);
    DVL_EXPECT_NEAR(midpoint.w, expectedMidpoint.w, Epsilon);
    DVL_EXPECT_NEAR(thirdQuarter.z, expectedThirdQuarter.z, Epsilon);
    DVL_EXPECT_NEAR(thirdQuarter.w, expectedThirdQuarter.w, Epsilon);
    DVL_EXPECT_NEAR(firstQuarter.Length(), 1.0f, Epsilon);
    DVL_EXPECT_NEAR(midpoint.Length(), 1.0f, Epsilon);
    DVL_EXPECT_NEAR(thirdQuarter.Length(), 1.0f, Epsilon);

    return true;
}

DVL_TEST(QuatSlerpHandlesNearlyIdenticalRotations)
{
    const dvl::Quat identity = dvl::Quat::Identity();
    const dvl::Quat smallRotation = dvl::Quat::FromAxisAngle(dvl::Vec3(1.0f, 0.0f, 0.0f), 0.001f);
    const dvl::Quat expected = dvl::Quat::FromAxisAngle(dvl::Vec3(1.0f, 0.0f, 0.0f), 0.0005f);
    const dvl::Quat midpoint = dvl::Slerp(identity, smallRotation, 0.5f);

    DVL_EXPECT_NEAR(midpoint.x, expected.x, Epsilon);
    DVL_EXPECT_NEAR(midpoint.y, expected.y, Epsilon);
    DVL_EXPECT_NEAR(midpoint.z, expected.z, Epsilon);
    DVL_EXPECT_NEAR(midpoint.w, expected.w, Epsilon);
    DVL_EXPECT_NEAR(midpoint.Length(), 1.0f, Epsilon);

    return true;
}

DVL_TEST(QuatLookRotationUsesCanonicalForwardAndDefaultUp)
{
    const dvl::Quat identity = dvl::Quat::LookRotation(dvl::Vec3(0.0f, 0.0f, -2.0f));
    DVL_EXPECT_NEAR(identity.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(identity.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(identity.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(identity.w, 1.0f, Epsilon);

    const dvl::Vec3 forward(2.0f, 3.0f, 4.0f);
    const dvl::Quat rotation = dvl::Quat::LookRotation(forward);
    const dvl::Vec4 rotated = dvl::Mat4::Rotation(rotation) * dvl::Vec4(0.0f, 0.0f, -1.0f, 0.0f);
    const dvl::Vec3 expected = forward.Normalized();
    DVL_EXPECT_NEAR(rotated.x, expected.x, Epsilon);
    DVL_EXPECT_NEAR(rotated.y, expected.y, Epsilon);
    DVL_EXPECT_NEAR(rotated.z, expected.z, Epsilon);
    DVL_EXPECT_TRUE(std::isfinite(rotation.Length()));
    DVL_EXPECT_NEAR(rotation.Length(), 1.0f, Epsilon);

    return true;
}

DVL_TEST(QuatLookRotationMatchesMatrixIncludingHalfTurnsAndCustomUp)
{
    const dvl::Vec3 forwards[] = {
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.00001f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f},
        {2.0f, 3.0f, 4.0f},
        {-3.0f, -1.0f, 2.0f}
    };
    const dvl::Vec3 ups[] = {
        {0.0f, -1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 3.0f},
        {1.0f, 5.0f, -2.0f},
        {2.0f, -4.0f, 1.0f}
    };

    for (int i = 0; i < 7; i++)
    {
        const dvl::Quat rotation = dvl::Quat::LookRotation(forwards[i], ups[i]);
        DVL_EXPECT_TRUE(std::isfinite(rotation.Length()));
        DVL_EXPECT_NEAR(rotation.Length(), 1.0f, Epsilon);
        const dvl::Mat4 actual = dvl::Mat4::Rotation(rotation);
        const dvl::Mat4 expected = dvl::Mat4::LookRotation(forwards[i], ups[i]);
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
                DVL_EXPECT_NEAR(actual[column][row], expected[column][row], Epsilon);
        }
    }

    return true;
}

DVL_TEST(QuatLookRotationReturnsIdentityForDegenerateDirections)
{
    const dvl::Quat rotations[] = {
        dvl::Quat::LookRotation(dvl::Vec3()),
        dvl::Quat::LookRotation(dvl::Vec3(0.0f, 0.0f, -1.0f), dvl::Vec3()),
        dvl::Quat::LookRotation(dvl::Vec3(0.0f, 2.0f, 0.0f)),
        dvl::Quat::LookRotation(dvl::Vec3(0.0f, -2.0f, 0.0f)),
        dvl::Quat::LookRotation(dvl::Vec3(1.0f, 2.0f, 3.0f), dvl::Vec3(2.0f, 4.0f, 6.0f))
    };
    for (const dvl::Quat& rotation : rotations)
    {
        DVL_EXPECT_EQ(rotation.x, 0.0f);
        DVL_EXPECT_EQ(rotation.y, 0.0f);
        DVL_EXPECT_EQ(rotation.z, 0.0f);
        DVL_EXPECT_EQ(rotation.w, 1.0f);
    }

    return true;
}

DVL_TEST(QuatFromMatrixReadsColumnMajorRotationAndIgnoresTranslation)
{
    dvl::Mat4 matrix;
    matrix[0][0] = 0.0f;
    matrix[0][1] = 1.0f;
    matrix[1][0] = -1.0f;
    matrix[1][1] = 0.0f;
    matrix[3][0] = 12.0f;
    matrix[3][1] = -3.0f;
    matrix[3][2] = 7.0f;

    const dvl::Quat rotation = dvl::Quat::FromMatrix(matrix);
    DVL_EXPECT_TRUE(std::isfinite(rotation.Length()));
    DVL_EXPECT_NEAR(rotation.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(rotation.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(rotation.z, std::sqrt(0.5f), Epsilon);
    DVL_EXPECT_NEAR(rotation.w, std::sqrt(0.5f), Epsilon);
    DVL_EXPECT_NEAR(rotation.Length(), 1.0f, Epsilon);

    return true;
}
