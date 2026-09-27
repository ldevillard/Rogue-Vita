#include "../unit_test.h"

#include "dvl/physics/intersection.h"

DVL_TEST(SphereObbHandlesContainmentAndZeroRadius)
{
    const dvl::Obb box{dvl::Vec3(3.0f, -2.0f, 5.0f), dvl::Vec3(1.0f, 2.0f, 3.0f), dvl::Quat::Identity()};

    DVL_EXPECT_TRUE(dvl::Intersects({box.center, 0.0f}, box));
    DVL_EXPECT_TRUE(dvl::Intersects({box.center, 10.0f}, box));
    DVL_EXPECT_TRUE(dvl::Intersects({dvl::Vec3(4.0f, -2.0f, 5.0f), 0.0f}, box));
    DVL_EXPECT_FALSE(dvl::Intersects({dvl::Vec3(4.1f, -2.0f, 5.0f), 0.0f}, box));

    return true;
}

DVL_TEST(SphereObbHandlesFaceEdgeAndCornerContacts)
{
    const dvl::Obb box{dvl::Vec3::Zero(), dvl::Vec3::One(), dvl::Quat::Identity()};
    const dvl::Sphere contacts[] = {
        {dvl::Vec3(2.0f, 0.0f, 0.0f), 1.0f},
        {dvl::Vec3(-2.0f, 0.0f, 0.0f), 1.0f},
        {dvl::Vec3(0.0f, 2.0f, 0.0f), 1.0f},
        {dvl::Vec3(0.0f, -2.0f, 0.0f), 1.0f},
        {dvl::Vec3(0.0f, 0.0f, 2.0f), 1.0f},
        {dvl::Vec3(0.0f, 0.0f, -2.0f), 1.0f},
        {dvl::Vec3(4.0f, 5.0f, 0.0f), 5.0f},
        {dvl::Vec3(2.0f, 3.0f, 3.0f), 3.0f}
    };

    for (const dvl::Sphere& sphere : contacts)
    {
        DVL_EXPECT_TRUE(dvl::Intersects(sphere, box));
        DVL_EXPECT_FALSE(dvl::Intersects({sphere.center, sphere.radius - 0.01f}, box));
        DVL_EXPECT_TRUE(dvl::Intersects({sphere.center, sphere.radius + 0.01f}, box));
    }

    return true;
}

DVL_TEST(SphereObbUsesOrientationAndWorldCenter)
{
    const dvl::Vec3 center(10.0f, -3.0f, 2.0f);
    const dvl::Obb box{
        center,
        dvl::Vec3(3.0f, 0.5f, 0.25f),
        dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), dvl::Radians(45.0f))
    };

    DVL_EXPECT_TRUE(dvl::Intersects({center + dvl::Vec3(1.5f, 1.5f, 0.0f), 0.1f}, box));
    DVL_EXPECT_FALSE(dvl::Intersects({center + dvl::Vec3(1.5f, -1.5f, 0.0f), 0.1f}, box));
    DVL_EXPECT_FALSE(dvl::Intersects({center + dvl::Vec3(0.0f, 0.0f, 0.5f), 0.1f}, box));

    return true;
}

DVL_TEST(SphereObbSupportsDegenerateBox)
{
    const dvl::Obb box{dvl::Vec3::Zero(), dvl::Vec3::Zero(), dvl::Quat::Identity()};

    DVL_EXPECT_TRUE(dvl::Intersects({dvl::Vec3::Zero(), 0.0f}, box));
    DVL_EXPECT_TRUE(dvl::Intersects({dvl::Vec3(3.0f, 4.0f, 0.0f), 5.0f}, box));
    DVL_EXPECT_FALSE(dvl::Intersects({dvl::Vec3(3.0f, 4.0f, 0.0f), 4.9f}, box));

    return true;
}
