#include "../unit_test.h"

#include "dvl/math/math.h"

namespace
{
    constexpr float Epsilon = 0.0001f;
}

DVL_TEST(MathAngleConstantsAndConversionsAreConstexpr)
{
    constexpr float HalfTurnRadians = dvl::Radians(180.0f);
    constexpr float HalfTurnDegrees = dvl::Degrees(dvl::Pi);

    static_assert(HalfTurnRadians > 3.14f && HalfTurnRadians < 3.15f);
    static_assert(HalfTurnDegrees > 179.99f && HalfTurnDegrees < 180.01f);

    DVL_EXPECT_NEAR(dvl::Pi, 3.14159265f, Epsilon);

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
