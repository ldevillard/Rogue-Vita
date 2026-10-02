#include "../unit_test.h"

#include "dvl/math/vec.h"

namespace
{
    constexpr float Epsilon = 0.0001f;
}

DVL_TEST(Vec2ConstructorsInitializeComponents)
{
    const dvl::Vec2 zero;
    DVL_EXPECT_EQ(zero.x, 0.0f);
    DVL_EXPECT_EQ(zero.y, 0.0f);

    const dvl::Vec2 value(1.0f, -2.0f);
    DVL_EXPECT_EQ(value.x, 1.0f);
    DVL_EXPECT_EQ(value.y, -2.0f);

    return true;
}

DVL_TEST(Vec2ZeroReturnsZeroVector)
{
    const dvl::Vec2 zero = dvl::Vec2::Zero();
    DVL_EXPECT_EQ(zero.x, 0.0f);
    DVL_EXPECT_EQ(zero.y, 0.0f);

    return true;
}

DVL_TEST(Vec2OneReturnsOneVector)
{
    const dvl::Vec2 one = dvl::Vec2::One();
    DVL_EXPECT_EQ(one.x, 1.0f);
    DVL_EXPECT_EQ(one.y, 1.0f);

    return true;
}

DVL_TEST(Vec2ArithmeticOperatorsReturnExpectedValues)
{
    const dvl::Vec2 a(2.0f, 4.0f);
    const dvl::Vec2 b(1.0f, 2.0f);

    const dvl::Vec2 sum = a + b;
    DVL_EXPECT_EQ(sum.x, 3.0f);
    DVL_EXPECT_EQ(sum.y, 6.0f);

    const dvl::Vec2 difference = a - b;
    DVL_EXPECT_EQ(difference.x, 1.0f);
    DVL_EXPECT_EQ(difference.y, 2.0f);

    const dvl::Vec2 product = b * 2.0f;
    DVL_EXPECT_EQ(product.x, 2.0f);
    DVL_EXPECT_EQ(product.y, 4.0f);

    const dvl::Vec2 quotient = a / 2.0f;
    DVL_EXPECT_EQ(quotient.x, 1.0f);
    DVL_EXPECT_EQ(quotient.y, 2.0f);

    return true;
}

DVL_TEST(Vec2CompoundOperatorsModifyTheVector)
{
    dvl::Vec2 value(1.0f, 2.0f);

    value += dvl::Vec2(2.0f, 3.0f);
    DVL_EXPECT_EQ(value.x, 3.0f);
    DVL_EXPECT_EQ(value.y, 5.0f);

    value -= dvl::Vec2(1.0f, 2.0f);
    DVL_EXPECT_EQ(value.x, 2.0f);
    DVL_EXPECT_EQ(value.y, 3.0f);

    value *= 2.0f;
    DVL_EXPECT_EQ(value.x, 4.0f);
    DVL_EXPECT_EQ(value.y, 6.0f);

    return true;
}

DVL_TEST(Vec2LengthAndNormalizationAreCorrect)
{
    const dvl::Vec2 value(3.0f, 4.0f);
    DVL_EXPECT_EQ(value.LengthSquared(), 25.0f);
    DVL_EXPECT_NEAR(value.Length(), 5.0f, Epsilon);

    const dvl::Vec2 normalized = value.Normalized();
    DVL_EXPECT_NEAR(normalized.x, 0.6f, Epsilon);
    DVL_EXPECT_NEAR(normalized.y, 0.8f, Epsilon);
    DVL_EXPECT_NEAR(normalized.Length(), 1.0f, Epsilon);

    dvl::Vec2 mutableValue(0.0f, 2.0f);
    mutableValue.Normalize();
    DVL_EXPECT_NEAR(mutableValue.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(mutableValue.y, 1.0f, Epsilon);

    const dvl::Vec2 normalizedZero = dvl::Vec2().Normalized();
    DVL_EXPECT_EQ(normalizedZero.x, 0.0f);
    DVL_EXPECT_EQ(normalizedZero.y, 0.0f);

    return true;
}

DVL_TEST(Vec2FreeFunctionsReturnExpectedValues)
{
    const dvl::Vec2 a(1.0f, 2.0f);
    const dvl::Vec2 b(3.0f, 4.0f);

    DVL_EXPECT_EQ(dvl::Dot(a, b), 11.0f);

    const dvl::Vec2 midpoint = dvl::Lerp(a, b, 0.5f);
    DVL_EXPECT_EQ(midpoint.x, 2.0f);
    DVL_EXPECT_EQ(midpoint.y, 3.0f);

    return true;
}

DVL_TEST(Vec2ReflectMirrorsIncidentAcrossUnitNormal)
{
    const dvl::Vec2 reflected = dvl::Reflect(dvl::Vec2(2.0f, -3.0f), dvl::Vec2(0.0f, 1.0f));
    DVL_EXPECT_EQ(reflected.x, 2.0f);
    DVL_EXPECT_EQ(reflected.y, 3.0f);

    const dvl::Vec2 diagonalNormal = dvl::Vec2(1.0f, 1.0f).Normalized();
    const dvl::Vec2 diagonalReflection = dvl::Reflect(dvl::Vec2(1.0f, 0.0f), diagonalNormal);
    DVL_EXPECT_NEAR(diagonalReflection.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(diagonalReflection.y, -1.0f, Epsilon);

    return true;
}

DVL_TEST(Vec2LerpSupportsSymmetryAndExtrapolation)
{
    const dvl::Vec2 a(-2.0f, 4.0f);
    const dvl::Vec2 b(6.0f, -2.0f);

    const dvl::Vec2 forward = dvl::Lerp(a, b, 0.35f);
    const dvl::Vec2 reverse = dvl::Lerp(b, a, 0.65f);
    DVL_EXPECT_NEAR(forward.x, reverse.x, Epsilon);
    DVL_EXPECT_NEAR(forward.y, reverse.y, Epsilon);

    const dvl::Vec2 extrapolated = dvl::Lerp(a, b, 1.5f);
    DVL_EXPECT_NEAR(extrapolated.x, 10.0f, Epsilon);
    DVL_EXPECT_NEAR(extrapolated.y, -5.0f, Epsilon);

    const dvl::Vec2 atStart = dvl::Lerp(a, b, 0.0f);
    const dvl::Vec2 atEnd = dvl::Lerp(a, b, 1.0f);
    DVL_EXPECT_EQ(atStart.x, a.x);
    DVL_EXPECT_EQ(atStart.y, a.y);
    DVL_EXPECT_EQ(atEnd.x, b.x);
    DVL_EXPECT_EQ(atEnd.y, b.y);

    return true;
}

DVL_TEST(Vec2NormalizationPreservesDirectionAndDotIdentities)
{
    const dvl::Vec2 vector(2.0f, -3.0f);
    const dvl::Vec2 normalized = vector.Normalized();
    const dvl::Vec2 scaledNormalized = (vector * 7.0f).Normalized();

    DVL_EXPECT_NEAR(dvl::Dot(vector, normalized), vector.Length(), Epsilon);
    DVL_EXPECT_NEAR(normalized.x, scaledNormalized.x, Epsilon);
    DVL_EXPECT_NEAR(normalized.y, scaledNormalized.y, Epsilon);
    DVL_EXPECT_NEAR(normalized.Length(), 1.0f, Epsilon);

    return true;
}

DVL_TEST(Vec3ConstructorsInitializeComponents)
{
    const dvl::Vec3 zero;
    DVL_EXPECT_EQ(zero.x, 0.0f);
    DVL_EXPECT_EQ(zero.y, 0.0f);
    DVL_EXPECT_EQ(zero.z, 0.0f);

    const dvl::Vec3 value(1.0f, -2.0f, 3.0f);
    DVL_EXPECT_EQ(value.x, 1.0f);
    DVL_EXPECT_EQ(value.y, -2.0f);
    DVL_EXPECT_EQ(value.z, 3.0f);

    return true;
}

DVL_TEST(Vec3ZeroReturnsZeroVector)
{
    const dvl::Vec3 zero = dvl::Vec3::Zero();
    DVL_EXPECT_EQ(zero.x, 0.0f);
    DVL_EXPECT_EQ(zero.y, 0.0f);
    DVL_EXPECT_EQ(zero.z, 0.0f);

    return true;
}

DVL_TEST(Vec3OneReturnsOneVector)
{
    const dvl::Vec3 one = dvl::Vec3::One();
    DVL_EXPECT_EQ(one.x, 1.0f);
    DVL_EXPECT_EQ(one.y, 1.0f);
    DVL_EXPECT_EQ(one.z, 1.0f);

    return true;
}

DVL_TEST(Vec3ArithmeticOperatorsReturnExpectedValues)
{
    const dvl::Vec3 a(2.0f, 4.0f, 6.0f);
    const dvl::Vec3 b(1.0f, 2.0f, 3.0f);

    const dvl::Vec3 sum = a + b;
    DVL_EXPECT_EQ(sum.x, 3.0f);
    DVL_EXPECT_EQ(sum.y, 6.0f);
    DVL_EXPECT_EQ(sum.z, 9.0f);

    const dvl::Vec3 difference = a - b;
    DVL_EXPECT_EQ(difference.x, 1.0f);
    DVL_EXPECT_EQ(difference.y, 2.0f);
    DVL_EXPECT_EQ(difference.z, 3.0f);

    const dvl::Vec3 product = b * 2.0f;
    DVL_EXPECT_EQ(product.x, 2.0f);
    DVL_EXPECT_EQ(product.y, 4.0f);
    DVL_EXPECT_EQ(product.z, 6.0f);

    const dvl::Vec3 quotient = a / 2.0f;
    DVL_EXPECT_EQ(quotient.x, 1.0f);
    DVL_EXPECT_EQ(quotient.y, 2.0f);
    DVL_EXPECT_EQ(quotient.z, 3.0f);

    return true;
}

DVL_TEST(Vec3CompoundOperatorsModifyTheVector)
{
    dvl::Vec3 value(1.0f, 2.0f, 3.0f);

    value += dvl::Vec3(2.0f, 3.0f, 4.0f);
    DVL_EXPECT_EQ(value.x, 3.0f);
    DVL_EXPECT_EQ(value.y, 5.0f);
    DVL_EXPECT_EQ(value.z, 7.0f);

    value -= dvl::Vec3(1.0f, 2.0f, 3.0f);
    DVL_EXPECT_EQ(value.x, 2.0f);
    DVL_EXPECT_EQ(value.y, 3.0f);
    DVL_EXPECT_EQ(value.z, 4.0f);

    value *= 2.0f;
    DVL_EXPECT_EQ(value.x, 4.0f);
    DVL_EXPECT_EQ(value.y, 6.0f);
    DVL_EXPECT_EQ(value.z, 8.0f);

    return true;
}

DVL_TEST(Vec3LengthAndNormalizationAreCorrect)
{
    const dvl::Vec3 value(3.0f, 4.0f, 0.0f);
    DVL_EXPECT_EQ(value.LengthSquared(), 25.0f);
    DVL_EXPECT_NEAR(value.Length(), 5.0f, Epsilon);

    const dvl::Vec3 normalized = value.Normalized();
    DVL_EXPECT_NEAR(normalized.x, 0.6f, Epsilon);
    DVL_EXPECT_NEAR(normalized.y, 0.8f, Epsilon);
    DVL_EXPECT_NEAR(normalized.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(normalized.Length(), 1.0f, Epsilon);

    dvl::Vec3 mutableValue(0.0f, 0.0f, 2.0f);
    mutableValue.Normalize();
    DVL_EXPECT_NEAR(mutableValue.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(mutableValue.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(mutableValue.z, 1.0f, Epsilon);

    const dvl::Vec3 normalizedZero = dvl::Vec3().Normalized();
    DVL_EXPECT_EQ(normalizedZero.x, 0.0f);
    DVL_EXPECT_EQ(normalizedZero.y, 0.0f);
    DVL_EXPECT_EQ(normalizedZero.z, 0.0f);

    return true;
}

DVL_TEST(Vec3FreeFunctionsReturnExpectedValues)
{
    const dvl::Vec3 xAxis(1.0f, 0.0f, 0.0f);
    const dvl::Vec3 yAxis(0.0f, 1.0f, 0.0f);

    DVL_EXPECT_EQ(dvl::Dot(xAxis, yAxis), 0.0f);
    DVL_EXPECT_EQ(dvl::Dot(dvl::Vec3(1.0f, 2.0f, 3.0f), dvl::Vec3(4.0f, 5.0f, 6.0f)), 32.0f);

    const dvl::Vec3 cross = dvl::Cross(xAxis, yAxis);
    DVL_EXPECT_EQ(cross.x, 0.0f);
    DVL_EXPECT_EQ(cross.y, 0.0f);
    DVL_EXPECT_EQ(cross.z, 1.0f);

    const dvl::Vec3 midpoint = dvl::Lerp(
        dvl::Vec3(0.0f, 2.0f, 4.0f),
        dvl::Vec3(2.0f, 4.0f, 6.0f),
        0.5f);
    DVL_EXPECT_EQ(midpoint.x, 1.0f);
    DVL_EXPECT_EQ(midpoint.y, 3.0f);
    DVL_EXPECT_EQ(midpoint.z, 5.0f);

    return true;
}

DVL_TEST(Vec3ReflectMirrorsIncidentAcrossUnitNormal)
{
    const dvl::Vec3 vec(2.0f, -3.0f, 4.0f);
    const dvl::Vec3 reflected = dvl::Reflect(vec, dvl::Vec3(0.0f, 1.0f, 0.0f));
    DVL_EXPECT_EQ(reflected.x, 2.0f);
    DVL_EXPECT_EQ(reflected.y, 3.0f);
    DVL_EXPECT_EQ(reflected.z, 4.0f);
    DVL_EXPECT_EQ(vec.y, -3.0f);

    return true;
}

DVL_TEST(Vec4ConstructorsInitializeComponents)
{
    const dvl::Vec4 zero;
    DVL_EXPECT_EQ(zero.x, 0.0f);
    DVL_EXPECT_EQ(zero.y, 0.0f);
    DVL_EXPECT_EQ(zero.z, 0.0f);
    DVL_EXPECT_EQ(zero.w, 0.0f);

    const dvl::Vec4 value(1.0f, -2.0f, 3.0f, -4.0f);
    DVL_EXPECT_EQ(value.x, 1.0f);
    DVL_EXPECT_EQ(value.y, -2.0f);
    DVL_EXPECT_EQ(value.z, 3.0f);
    DVL_EXPECT_EQ(value.w, -4.0f);

    return true;
}

DVL_TEST(Vec4ZeroReturnsZeroVector)
{
    const dvl::Vec4 zero = dvl::Vec4::Zero();
    DVL_EXPECT_EQ(zero.x, 0.0f);
    DVL_EXPECT_EQ(zero.y, 0.0f);
    DVL_EXPECT_EQ(zero.z, 0.0f);
    DVL_EXPECT_EQ(zero.w, 0.0f);

    return true;
}

DVL_TEST(Vec4OneReturnsOneVector)
{
    const dvl::Vec4 one = dvl::Vec4::One();
    DVL_EXPECT_EQ(one.x, 1.0f);
    DVL_EXPECT_EQ(one.y, 1.0f);
    DVL_EXPECT_EQ(one.z, 1.0f);
    DVL_EXPECT_EQ(one.w, 1.0f);

    return true;
}

DVL_TEST(Vec4XYZReturnsFirstThreeComponentsAndIgnoresW)
{
    const dvl::Vec4 first(1.0f, -2.0f, 3.0f, 42.0f);
    const dvl::Vec4 second(1.0f, -2.0f, 3.0f, -99.0f);
    const dvl::Vec3 firstXYZ = first.XYZ();
    const dvl::Vec3 secondXYZ = second.XYZ();

    DVL_EXPECT_EQ(firstXYZ.x, 1.0f);
    DVL_EXPECT_EQ(firstXYZ.y, -2.0f);
    DVL_EXPECT_EQ(firstXYZ.z, 3.0f);
    DVL_EXPECT_EQ(secondXYZ.x, firstXYZ.x);
    DVL_EXPECT_EQ(secondXYZ.y, firstXYZ.y);
    DVL_EXPECT_EQ(secondXYZ.z, firstXYZ.z);

    return true;
}

DVL_TEST(Vec4ArithmeticOperatorsReturnExpectedValues)
{
    const dvl::Vec4 a(2.0f, 4.0f, 6.0f, 8.0f);
    const dvl::Vec4 b(1.0f, 2.0f, 3.0f, 4.0f);

    const dvl::Vec4 sum = a + b;
    DVL_EXPECT_EQ(sum.x, 3.0f);
    DVL_EXPECT_EQ(sum.y, 6.0f);
    DVL_EXPECT_EQ(sum.z, 9.0f);
    DVL_EXPECT_EQ(sum.w, 12.0f);

    const dvl::Vec4 difference = a - b;
    DVL_EXPECT_EQ(difference.x, 1.0f);
    DVL_EXPECT_EQ(difference.y, 2.0f);
    DVL_EXPECT_EQ(difference.z, 3.0f);
    DVL_EXPECT_EQ(difference.w, 4.0f);

    const dvl::Vec4 product = b * 2.0f;
    DVL_EXPECT_EQ(product.x, 2.0f);
    DVL_EXPECT_EQ(product.y, 4.0f);
    DVL_EXPECT_EQ(product.z, 6.0f);
    DVL_EXPECT_EQ(product.w, 8.0f);

    const dvl::Vec4 quotient = a / 2.0f;
    DVL_EXPECT_EQ(quotient.x, 1.0f);
    DVL_EXPECT_EQ(quotient.y, 2.0f);
    DVL_EXPECT_EQ(quotient.z, 3.0f);
    DVL_EXPECT_EQ(quotient.w, 4.0f);

    return true;
}

DVL_TEST(Vec4CompoundOperatorsModifyTheVector)
{
    dvl::Vec4 value(1.0f, 2.0f, 3.0f, 4.0f);

    value += dvl::Vec4(2.0f, 3.0f, 4.0f, 5.0f);
    DVL_EXPECT_EQ(value.x, 3.0f);
    DVL_EXPECT_EQ(value.y, 5.0f);
    DVL_EXPECT_EQ(value.z, 7.0f);
    DVL_EXPECT_EQ(value.w, 9.0f);

    value -= dvl::Vec4(1.0f, 2.0f, 3.0f, 4.0f);
    DVL_EXPECT_EQ(value.x, 2.0f);
    DVL_EXPECT_EQ(value.y, 3.0f);
    DVL_EXPECT_EQ(value.z, 4.0f);
    DVL_EXPECT_EQ(value.w, 5.0f);

    value *= 2.0f;
    DVL_EXPECT_EQ(value.x, 4.0f);
    DVL_EXPECT_EQ(value.y, 6.0f);
    DVL_EXPECT_EQ(value.z, 8.0f);
    DVL_EXPECT_EQ(value.w, 10.0f);

    return true;
}

DVL_TEST(Vec4LengthAndNormalizationAreCorrect)
{
    const dvl::Vec4 value(1.0f, 2.0f, 2.0f, 0.0f);
    DVL_EXPECT_EQ(value.LengthSquared(), 9.0f);
    DVL_EXPECT_NEAR(value.Length(), 3.0f, Epsilon);

    const dvl::Vec4 normalized = value.Normalized();
    DVL_EXPECT_NEAR(normalized.x, 1.0f / 3.0f, Epsilon);
    DVL_EXPECT_NEAR(normalized.y, 2.0f / 3.0f, Epsilon);
    DVL_EXPECT_NEAR(normalized.z, 2.0f / 3.0f, Epsilon);
    DVL_EXPECT_NEAR(normalized.w, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(normalized.Length(), 1.0f, Epsilon);

    dvl::Vec4 mutableValue(0.0f, 0.0f, 0.0f, 2.0f);
    mutableValue.Normalize();
    DVL_EXPECT_NEAR(mutableValue.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(mutableValue.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(mutableValue.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(mutableValue.w, 1.0f, Epsilon);

    const dvl::Vec4 normalizedZero = dvl::Vec4().Normalized();
    DVL_EXPECT_EQ(normalizedZero.x, 0.0f);
    DVL_EXPECT_EQ(normalizedZero.y, 0.0f);
    DVL_EXPECT_EQ(normalizedZero.z, 0.0f);
    DVL_EXPECT_EQ(normalizedZero.w, 0.0f);

    return true;
}

DVL_TEST(Vec4FreeFunctionsReturnExpectedValues)
{
    const dvl::Vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
    const dvl::Vec4 b(5.0f, 6.0f, 7.0f, 8.0f);

    DVL_EXPECT_EQ(dvl::Dot(a, b), 70.0f);

    const dvl::Vec4 midpoint = dvl::Lerp(a, b, 0.5f);
    DVL_EXPECT_EQ(midpoint.x, 3.0f);
    DVL_EXPECT_EQ(midpoint.y, 4.0f);
    DVL_EXPECT_EQ(midpoint.z, 5.0f);
    DVL_EXPECT_EQ(midpoint.w, 6.0f);

    return true;
}

DVL_TEST(Vec4ReflectMirrorsIncidentAcrossUnitNormal)
{
    const dvl::Vec4 reflected = dvl::Reflect(
        dvl::Vec4(1.0f, 2.0f, 3.0f, -4.0f),
        dvl::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    DVL_EXPECT_EQ(reflected.x, 1.0f);
    DVL_EXPECT_EQ(reflected.y, 2.0f);
    DVL_EXPECT_EQ(reflected.z, 3.0f);
    DVL_EXPECT_EQ(reflected.w, 4.0f);

    return true;
}

DVL_TEST(Vec3CrossProductSatisfiesGeometricIdentities)
{
    const dvl::Vec3 a(2.0f, -1.0f, 3.0f);
    const dvl::Vec3 b(-4.0f, 5.0f, 2.0f);
    const dvl::Vec3 cross = dvl::Cross(a, b);

    DVL_EXPECT_NEAR(dvl::Dot(cross, a), 0.0f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Dot(cross, b), 0.0f, Epsilon);
    DVL_EXPECT_NEAR(
        cross.LengthSquared(),
        a.LengthSquared() * b.LengthSquared() - dvl::Dot(a, b) * dvl::Dot(a, b),
        Epsilon);

    const dvl::Vec3 reverseCross = dvl::Cross(b, a);
    DVL_EXPECT_NEAR(reverseCross.x, -cross.x, Epsilon);
    DVL_EXPECT_NEAR(reverseCross.y, -cross.y, Epsilon);
    DVL_EXPECT_NEAR(reverseCross.z, -cross.z, Epsilon);

    const dvl::Vec3 parallelCross = dvl::Cross(a, a * 3.0f);
    DVL_EXPECT_NEAR(parallelCross.LengthSquared(), 0.0f, Epsilon);

    return true;
}

DVL_TEST(Vec3LerpSupportsSymmetryAndExtrapolation)
{
    const dvl::Vec3 a(-2.0f, 4.0f, 1.0f);
    const dvl::Vec3 b(6.0f, -2.0f, 5.0f);

    const dvl::Vec3 forward = dvl::Lerp(a, b, 0.35f);
    const dvl::Vec3 reverse = dvl::Lerp(b, a, 0.65f);
    DVL_EXPECT_NEAR(forward.x, reverse.x, Epsilon);
    DVL_EXPECT_NEAR(forward.y, reverse.y, Epsilon);
    DVL_EXPECT_NEAR(forward.z, reverse.z, Epsilon);

    const dvl::Vec3 extrapolated = dvl::Lerp(a, b, 1.5f);
    DVL_EXPECT_NEAR(extrapolated.x, 10.0f, Epsilon);
    DVL_EXPECT_NEAR(extrapolated.y, -5.0f, Epsilon);
    DVL_EXPECT_NEAR(extrapolated.z, 7.0f, Epsilon);

    const dvl::Vec3 atStart = dvl::Lerp(a, b, 0.0f);
    const dvl::Vec3 atEnd = dvl::Lerp(a, b, 1.0f);
    DVL_EXPECT_EQ(atStart.x, a.x);
    DVL_EXPECT_EQ(atStart.y, a.y);
    DVL_EXPECT_EQ(atStart.z, a.z);
    DVL_EXPECT_EQ(atEnd.x, b.x);
    DVL_EXPECT_EQ(atEnd.y, b.y);
    DVL_EXPECT_EQ(atEnd.z, b.z);

    return true;
}

DVL_TEST(VecNormalizationPreservesDirectionAndDotIdentities)
{
    const dvl::Vec3 vector3(2.0f, -3.0f, 6.0f);
    const dvl::Vec3 normalized3 = vector3.Normalized();
    const dvl::Vec3 scaledNormalized3 = (vector3 * 7.0f).Normalized();
    DVL_EXPECT_NEAR(dvl::Cross(vector3, normalized3).LengthSquared(), 0.0f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Dot(vector3, normalized3), vector3.Length(), Epsilon);
    DVL_EXPECT_NEAR(normalized3.x, scaledNormalized3.x, Epsilon);
    DVL_EXPECT_NEAR(normalized3.y, scaledNormalized3.y, Epsilon);
    DVL_EXPECT_NEAR(normalized3.z, scaledNormalized3.z, Epsilon);

    const dvl::Vec4 vector4(1.0f, -2.0f, 3.0f, -4.0f);
    const dvl::Vec4 normalized4 = vector4.Normalized();
    DVL_EXPECT_NEAR(dvl::Dot(vector4, vector4), vector4.LengthSquared(), Epsilon);
    DVL_EXPECT_NEAR(dvl::Dot(vector4, normalized4), vector4.Length(), Epsilon);
    DVL_EXPECT_NEAR(normalized4.Length(), 1.0f, Epsilon);

    return true;
}

DVL_TEST(Vec4LerpSupportsSymmetryAndExtrapolation)
{
    const dvl::Vec4 a(-1.0f, 2.0f, -3.0f, 4.0f);
    const dvl::Vec4 b(3.0f, -2.0f, 5.0f, 0.0f);

    const dvl::Vec4 forward = dvl::Lerp(a, b, 0.2f);
    const dvl::Vec4 reverse = dvl::Lerp(b, a, 0.8f);
    DVL_EXPECT_NEAR(forward.x, reverse.x, Epsilon);
    DVL_EXPECT_NEAR(forward.y, reverse.y, Epsilon);
    DVL_EXPECT_NEAR(forward.z, reverse.z, Epsilon);
    DVL_EXPECT_NEAR(forward.w, reverse.w, Epsilon);

    const dvl::Vec4 extrapolated = dvl::Lerp(a, b, -0.5f);
    DVL_EXPECT_NEAR(extrapolated.x, -3.0f, Epsilon);
    DVL_EXPECT_NEAR(extrapolated.y, 4.0f, Epsilon);
    DVL_EXPECT_NEAR(extrapolated.z, -7.0f, Epsilon);
    DVL_EXPECT_NEAR(extrapolated.w, 6.0f, Epsilon);

    return true;
}

DVL_TEST(Vec2AbsReturnsMagnitudesWithoutChangingInput)
{
    dvl::Vec2 value(-1.5f, 2.5f);
    const dvl::Vec2 result = dvl::Abs(value);
    const dvl::Vec2 oppositeResult = dvl::Abs(value * -1.0f);

    DVL_EXPECT_EQ(result.x, 1.5f);
    DVL_EXPECT_EQ(oppositeResult.x, 1.5f);
    DVL_EXPECT_EQ(value.x, -1.5f);
    DVL_EXPECT_EQ(result.y, 2.5f);
    DVL_EXPECT_EQ(oppositeResult.y, 2.5f);
    DVL_EXPECT_EQ(value.y, 2.5f);

    return true;
}

DVL_TEST(Vec2AbsConvertsSignedZerosToPositiveZero)
{
    for (float zero : {0.0f, -0.0f})
    {
        const dvl::Vec2 value(zero, zero);
        const dvl::Vec2 result = dvl::Abs(value);

        DVL_EXPECT_EQ(result.x, 0.0f);
        DVL_EXPECT_FALSE(std::signbit(result.x));
        DVL_EXPECT_EQ(std::signbit(value.x), std::signbit(zero));
        DVL_EXPECT_EQ(result.y, 0.0f);
        DVL_EXPECT_FALSE(std::signbit(result.y));
        DVL_EXPECT_EQ(std::signbit(value.y), std::signbit(zero));
    }

    return true;
}

DVL_TEST(Vec3AbsReturnsMagnitudesWithoutChangingInput)
{
    dvl::Vec3 value(-1.5f, 2.5f, -3.5f);
    const dvl::Vec3 result = dvl::Abs(value);
    const dvl::Vec3 oppositeResult = dvl::Abs(value * -1.0f);

    DVL_EXPECT_EQ(result.x, 1.5f);
    DVL_EXPECT_EQ(oppositeResult.x, 1.5f);
    DVL_EXPECT_EQ(value.x, -1.5f);
    DVL_EXPECT_EQ(result.y, 2.5f);
    DVL_EXPECT_EQ(oppositeResult.y, 2.5f);
    DVL_EXPECT_EQ(value.y, 2.5f);
    DVL_EXPECT_EQ(result.z, 3.5f);
    DVL_EXPECT_EQ(oppositeResult.z, 3.5f);
    DVL_EXPECT_EQ(value.z, -3.5f);

    return true;
}

DVL_TEST(Vec3AbsConvertsSignedZerosToPositiveZero)
{
    for (float zero : {0.0f, -0.0f})
    {
        const dvl::Vec3 value(zero, zero, zero);
        const dvl::Vec3 result = dvl::Abs(value);

        DVL_EXPECT_EQ(result.x, 0.0f);
        DVL_EXPECT_FALSE(std::signbit(result.x));
        DVL_EXPECT_EQ(std::signbit(value.x), std::signbit(zero));
        DVL_EXPECT_EQ(result.y, 0.0f);
        DVL_EXPECT_FALSE(std::signbit(result.y));
        DVL_EXPECT_EQ(std::signbit(value.y), std::signbit(zero));
        DVL_EXPECT_EQ(result.z, 0.0f);
        DVL_EXPECT_FALSE(std::signbit(result.z));
        DVL_EXPECT_EQ(std::signbit(value.z), std::signbit(zero));
    }

    return true;
}

DVL_TEST(Vec4AbsReturnsMagnitudesWithoutChangingInput)
{
    dvl::Vec4 value(-1.5f, 2.5f, -3.5f, 4.5f);
    const dvl::Vec4 result = dvl::Abs(value);
    const dvl::Vec4 oppositeResult = dvl::Abs(value * -1.0f);

    DVL_EXPECT_EQ(result.x, 1.5f);
    DVL_EXPECT_EQ(oppositeResult.x, 1.5f);
    DVL_EXPECT_EQ(value.x, -1.5f);
    DVL_EXPECT_EQ(result.y, 2.5f);
    DVL_EXPECT_EQ(oppositeResult.y, 2.5f);
    DVL_EXPECT_EQ(value.y, 2.5f);
    DVL_EXPECT_EQ(result.z, 3.5f);
    DVL_EXPECT_EQ(oppositeResult.z, 3.5f);
    DVL_EXPECT_EQ(value.z, -3.5f);
    DVL_EXPECT_EQ(result.w, 4.5f);
    DVL_EXPECT_EQ(oppositeResult.w, 4.5f);
    DVL_EXPECT_EQ(value.w, 4.5f);

    return true;
}

DVL_TEST(Vec4AbsConvertsSignedZerosToPositiveZero)
{
    for (float zero : {0.0f, -0.0f})
    {
        const dvl::Vec4 value(zero, zero, zero, zero);
        const dvl::Vec4 result = dvl::Abs(value);

        DVL_EXPECT_EQ(result.x, 0.0f);
        DVL_EXPECT_FALSE(std::signbit(result.x));
        DVL_EXPECT_EQ(std::signbit(value.x), std::signbit(zero));
        DVL_EXPECT_EQ(result.y, 0.0f);
        DVL_EXPECT_FALSE(std::signbit(result.y));
        DVL_EXPECT_EQ(std::signbit(value.y), std::signbit(zero));
        DVL_EXPECT_EQ(result.z, 0.0f);
        DVL_EXPECT_FALSE(std::signbit(result.z));
        DVL_EXPECT_EQ(std::signbit(value.z), std::signbit(zero));
        DVL_EXPECT_EQ(result.w, 0.0f);
        DVL_EXPECT_FALSE(std::signbit(result.w));
        DVL_EXPECT_EQ(std::signbit(value.w), std::signbit(zero));
    }

    return true;
}

DVL_TEST(Vec2ComponentMultiplicationPreservesOperandsAndIdentities)
{
    dvl::Vec2 a(-2.0f, 3.0f);
    dvl::Vec2 b(-0.5f, -4.0f);
    const dvl::Vec2 product = a * b;
    const dvl::Vec2 reverse = b * a;
    const dvl::Vec2 identity = a * dvl::Vec2::One();
    const dvl::Vec2 zero = a * dvl::Vec2::Zero();

    DVL_EXPECT_EQ(product.x, 1.0f);
    DVL_EXPECT_EQ(reverse.x, product.x);
    DVL_EXPECT_EQ(identity.x, -2.0f);
    DVL_EXPECT_EQ(zero.x, 0.0f);
    DVL_EXPECT_EQ(a.x, -2.0f);
    DVL_EXPECT_EQ(b.x, -0.5f);
    DVL_EXPECT_EQ(product.y, -12.0f);
    DVL_EXPECT_EQ(reverse.y, product.y);
    DVL_EXPECT_EQ(identity.y, 3.0f);
    DVL_EXPECT_EQ(zero.y, 0.0f);
    DVL_EXPECT_EQ(a.y, 3.0f);
    DVL_EXPECT_EQ(b.y, -4.0f);

    return true;
}

DVL_TEST(Vec3ComponentMultiplicationPreservesOperandsAndIdentities)
{
    dvl::Vec3 a(-2.0f, 3.0f, -4.0f);
    dvl::Vec3 b(-0.5f, -4.0f, 2.0f);
    const dvl::Vec3 product = a * b;
    const dvl::Vec3 reverse = b * a;
    const dvl::Vec3 identity = a * dvl::Vec3::One();
    const dvl::Vec3 zero = a * dvl::Vec3::Zero();

    DVL_EXPECT_EQ(product.x, 1.0f);
    DVL_EXPECT_EQ(reverse.x, product.x);
    DVL_EXPECT_EQ(identity.x, -2.0f);
    DVL_EXPECT_EQ(zero.x, 0.0f);
    DVL_EXPECT_EQ(a.x, -2.0f);
    DVL_EXPECT_EQ(b.x, -0.5f);
    DVL_EXPECT_EQ(product.y, -12.0f);
    DVL_EXPECT_EQ(reverse.y, product.y);
    DVL_EXPECT_EQ(identity.y, 3.0f);
    DVL_EXPECT_EQ(zero.y, 0.0f);
    DVL_EXPECT_EQ(a.y, 3.0f);
    DVL_EXPECT_EQ(b.y, -4.0f);
    DVL_EXPECT_EQ(product.z, -8.0f);
    DVL_EXPECT_EQ(reverse.z, product.z);
    DVL_EXPECT_EQ(identity.z, -4.0f);
    DVL_EXPECT_EQ(zero.z, 0.0f);
    DVL_EXPECT_EQ(a.z, -4.0f);
    DVL_EXPECT_EQ(b.z, 2.0f);

    return true;
}

DVL_TEST(Vec4ComponentMultiplicationPreservesOperandsAndIdentities)
{
    dvl::Vec4 a(-2.0f, 3.0f, -4.0f, 0.5f);
    dvl::Vec4 b(-0.5f, -4.0f, 2.0f, 6.0f);
    const dvl::Vec4 product = a * b;
    const dvl::Vec4 reverse = b * a;
    const dvl::Vec4 identity = a * dvl::Vec4::One();
    const dvl::Vec4 zero = a * dvl::Vec4::Zero();

    DVL_EXPECT_EQ(product.x, 1.0f);
    DVL_EXPECT_EQ(reverse.x, product.x);
    DVL_EXPECT_EQ(identity.x, -2.0f);
    DVL_EXPECT_EQ(zero.x, 0.0f);
    DVL_EXPECT_EQ(a.x, -2.0f);
    DVL_EXPECT_EQ(b.x, -0.5f);
    DVL_EXPECT_EQ(product.y, -12.0f);
    DVL_EXPECT_EQ(reverse.y, product.y);
    DVL_EXPECT_EQ(identity.y, 3.0f);
    DVL_EXPECT_EQ(zero.y, 0.0f);
    DVL_EXPECT_EQ(a.y, 3.0f);
    DVL_EXPECT_EQ(b.y, -4.0f);
    DVL_EXPECT_EQ(product.z, -8.0f);
    DVL_EXPECT_EQ(reverse.z, product.z);
    DVL_EXPECT_EQ(identity.z, -4.0f);
    DVL_EXPECT_EQ(zero.z, 0.0f);
    DVL_EXPECT_EQ(a.z, -4.0f);
    DVL_EXPECT_EQ(b.z, 2.0f);
    DVL_EXPECT_EQ(product.w, 3.0f);
    DVL_EXPECT_EQ(reverse.w, product.w);
    DVL_EXPECT_EQ(identity.w, 0.5f);
    DVL_EXPECT_EQ(zero.w, 0.0f);
    DVL_EXPECT_EQ(a.w, 0.5f);
    DVL_EXPECT_EQ(b.w, 6.0f);

    return true;
}

DVL_TEST(Vec3ComponentMultiplicationCombinesWithAbsAndScalarForHalfExtents)
{
    const dvl::Vec3 size(4.0f, 2.0f, 6.0f);
    const dvl::Vec3 scale(-2.0f, 0.5f, -3.0f);
    const dvl::Vec3 halfExtents = size * dvl::Abs(scale) * 0.5f;

    DVL_EXPECT_EQ(halfExtents.x, 4.0f);
    DVL_EXPECT_EQ(halfExtents.y, 0.5f);
    DVL_EXPECT_EQ(halfExtents.z, 9.0f);

    return true;
}
