#include "unit_test.h"

#include "engine/core/transform.h"

namespace
{
    constexpr float Tolerance = 0.0001f;
}

DVL_TEST(TransformDefaultsToIdentity)
{
    const Transform transform;

    DVL_EXPECT_EQ(transform.position.x, 0.0f);
    DVL_EXPECT_EQ(transform.position.y, 0.0f);
    DVL_EXPECT_EQ(transform.position.z, 0.0f);

    DVL_EXPECT_EQ(transform.rotation.x, 0.0f);
    DVL_EXPECT_EQ(transform.rotation.y, 0.0f);
    DVL_EXPECT_EQ(transform.rotation.z, 0.0f);
    DVL_EXPECT_EQ(transform.rotation.w, 1.0f);

    DVL_EXPECT_EQ(transform.scale.x, 1.0f);
    DVL_EXPECT_EQ(transform.scale.y, 1.0f);
    DVL_EXPECT_EQ(transform.scale.z, 1.0f);

    return true;
}

DVL_TEST(TransformPointAppliesScaleRotationAndTranslation)
{
    Transform transform;
    transform.position = dvl::Vec3(10.0f, 20.0f, 30.0f);
    transform.rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), dvl::Pi / 2.0f);
    transform.scale = dvl::Vec3(2.0f, 3.0f, 4.0f);

    const dvl::Vec3 point = transform.TransformPoint(dvl::Vec3(1.0f, 2.0f, 3.0f));

    DVL_EXPECT_NEAR(point.x, 4.0f, Tolerance);
    DVL_EXPECT_NEAR(point.y, 22.0f, Tolerance);
    DVL_EXPECT_NEAR(point.z, 42.0f, Tolerance);

    return true;
}

DVL_TEST(TransformLookDirectionUpdatesLocalAxes)
{
    Transform transform;
    transform.LookDirection(dvl::Vec3(1.0f, 0.0f, 0.0f));

    const dvl::Vec3 forward = transform.GetForward();
    const dvl::Vec3 right = transform.GetRight();
    const dvl::Vec3 up = transform.GetUp();

    DVL_EXPECT_NEAR(forward.x, 1.0f, Tolerance);
    DVL_EXPECT_NEAR(forward.y, 0.0f, Tolerance);
    DVL_EXPECT_NEAR(forward.z, 0.0f, Tolerance);

    DVL_EXPECT_NEAR(dvl::Dot(forward, right), 0.0f, Tolerance);
    DVL_EXPECT_NEAR(dvl::Dot(forward, up), 0.0f, Tolerance);
    DVL_EXPECT_NEAR(dvl::Dot(right, up), 0.0f, Tolerance);

    return true;
}
