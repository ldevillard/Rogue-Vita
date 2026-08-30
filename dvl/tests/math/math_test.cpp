#include "../unit_test.h"

#include "dvl/math/math.h"

namespace
{
    constexpr float Epsilon = 0.0001f;
}

DVL_TEST(PiIsConstexprAndAngleConversionsAreCorrect)
{
    constexpr float ExpectedPi = dvl::Pi;
    const float halfTurnRadians = dvl::Radians(180.0f);
    const float halfTurnDegrees = dvl::Degrees(dvl::Pi);

    static_assert(ExpectedPi > 3.14f && ExpectedPi < 3.15f);

    DVL_EXPECT_NEAR(dvl::Pi, 3.14159265f, Epsilon);
    DVL_EXPECT_NEAR(halfTurnRadians, dvl::Pi, Epsilon);
    DVL_EXPECT_NEAR(halfTurnDegrees, 180.0f, Epsilon);

    return true;
}

DVL_TEST(RadiansConvertsDegreesToRadians)
{
    DVL_EXPECT_NEAR(dvl::Radians(0.0f), 0.0f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Radians(45.0f), dvl::Pi * 0.25f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Radians(90.0f), dvl::Pi * 0.5f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Radians(180.0f), dvl::Pi, Epsilon);
    DVL_EXPECT_NEAR(dvl::Radians(360.0f), dvl::Pi * 2.0f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Radians(-90.0f), -dvl::Pi * 0.5f, Epsilon);

    return true;
}

DVL_TEST(DegreesConvertsRadiansToDegrees)
{
    DVL_EXPECT_NEAR(dvl::Degrees(0.0f), 0.0f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Degrees(dvl::Pi * 0.25f), 45.0f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Degrees(dvl::Pi * 0.5f), 90.0f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Degrees(dvl::Pi), 180.0f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Degrees(dvl::Pi * 2.0f), 360.0f, Epsilon);
    DVL_EXPECT_NEAR(dvl::Degrees(-dvl::Pi * 0.5f), -90.0f, Epsilon);

    return true;
}

DVL_TEST(DegreesAndRadiansRoundTrip)
{
    constexpr float Angles[] = {-720.0f, -135.0f, -0.5f, 0.0f, 12.5f, 180.0f, 540.0f};

    for (const float angle : Angles)
        DVL_EXPECT_NEAR(dvl::Degrees(dvl::Radians(angle)), angle, Epsilon);

    return true;
}

DVL_TEST(ClampRestrictsValuesToTheRequestedRange)
{
    const float below = dvl::Clamp(-2.0f, -1.0f, 1.0f);
    const float inside = dvl::Clamp(0.25f, -1.0f, 1.0f);
    const float above = dvl::Clamp(3.0f, -1.0f, 1.0f);

    DVL_EXPECT_EQ(below, -1.0f);
    DVL_EXPECT_EQ(inside, 0.25f);
    DVL_EXPECT_EQ(above, 1.0f);

    return true;
}

DVL_TEST(InverseSqrtReturnsReciprocalSquareRoot)
{
    DVL_EXPECT_NEAR(dvl::InverseSqrt(1.0f), 1.0f, Epsilon);
    DVL_EXPECT_NEAR(dvl::InverseSqrt(4.0f), 0.5f, Epsilon);
    DVL_EXPECT_NEAR(dvl::InverseSqrt(16.0f), 0.25f, Epsilon);
    DVL_EXPECT_NEAR(dvl::InverseSqrt(0.25f), 2.0f, Epsilon);

    return true;
}

DVL_TEST(InverseSqrtIsTheMultiplicativeInverseOfSqrt)
{
    constexpr float Values[] = {0.01f, 0.5f, 2.0f, 10.0f, 100.0f};

    for (const float value : Values)
        DVL_EXPECT_NEAR(dvl::InverseSqrt(value) * std::sqrt(value), 1.0f, Epsilon);

    return true;
}
