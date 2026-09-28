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

DVL_TEST(RayObbReturnsNearestSurfaceAndOutwardNormal)
{
    const dvl::Obb box{{0.0f, 0.0f, 0.0f}, dvl::Vec3::One(), dvl::Quat::Identity()};
    float distance;
    dvl::Vec3 normal;
    for (float sign : {-1.0f, 1.0f})
    {
        const dvl::Ray ray{{3.0f * sign, 0.0f, 0.0f}, {-sign, 0.0f, 0.0f}};
        DVL_EXPECT_TRUE(dvl::Intersects(ray, box, distance, normal));
        DVL_EXPECT_NEAR(distance, 2.0f, 0.0001f);
        DVL_EXPECT_NEAR(normal.x, sign, 0.0001f);
        DVL_EXPECT_NEAR(normal.y, 0.0f, 0.0001f);
        DVL_EXPECT_NEAR(normal.z, 0.0f, 0.0001f);
    }
    return true;
}

DVL_TEST(RayObbHandlesMissesParallelRaysAndZeroDirection)
{
    const dvl::Obb box{dvl::Vec3::Zero(), dvl::Vec3::One(), dvl::Quat::Identity()};
    const dvl::Ray misses[] = {
        {{-3.0f, 2.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{3.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        {{-3.0f, 0.0f, 0.0f}, dvl::Vec3(1.0f, 2.0f, 0.0f).Normalized()}
    };
    for (const auto& ray : misses)
    {
        float distance = 42.0f;
        dvl::Vec3 normal = dvl::Vec3::One();
        DVL_EXPECT_FALSE(dvl::Intersects(ray, box, distance, normal));
        DVL_EXPECT_EQ(distance, 42.0f);
        DVL_EXPECT_EQ(normal.x, 1.0f);
        DVL_EXPECT_EQ(normal.y, 1.0f);
        DVL_EXPECT_EQ(normal.z, 1.0f);
    }
    return true;
}

DVL_TEST(RayObbHandlesInsideBoundaryAndTangency)
{
    const dvl::Obb box{dvl::Vec3::Zero(), dvl::Vec3::One(), dvl::Quat::Identity()};
    float distance;
    dvl::Vec3 normal;
    DVL_EXPECT_TRUE(dvl::Intersects(dvl::Ray{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, box, distance, normal));
    DVL_EXPECT_NEAR(distance, 1.0f, 0.0001f);
    DVL_EXPECT_NEAR(normal.y, 1.0f, 0.0001f);
    for (float sign : {-1.0f, 1.0f})
    {
        DVL_EXPECT_TRUE(dvl::Intersects(dvl::Ray{{1.0f, 0.0f, 0.0f}, {sign, 0.0f, 0.0f}}, box, distance, normal));
        DVL_EXPECT_NEAR(distance, 0.0f, 0.0001f);
        DVL_EXPECT_NEAR(normal.x, 1.0f, 0.0001f);
    }
    DVL_EXPECT_TRUE(dvl::Intersects(dvl::Ray{{-3.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}}, box, distance, normal));
    DVL_EXPECT_NEAR(distance, 2.0f, 0.0001f);
    return true;
}

DVL_TEST(RayObbUsesRotationAndTranslation)
{
    const dvl::Obb box{
        {10.0f, -3.0f, 2.0f}, {2.0f, 0.5f, 1.0f},
        dvl::Quat::FromAxisAngle({0.0f, 0.0f, 1.0f}, dvl::Radians(45.0f))
    };
    const dvl::Vec3 axis = dvl::Vec3(1.0f, 1.0f, 0.0f).Normalized();
    const dvl::Ray ray{box.center + axis * 5.0f, axis * -1.0f};
    float distance;
    dvl::Vec3 normal;
    DVL_EXPECT_TRUE(dvl::Intersects(ray, box, distance, normal));
    DVL_EXPECT_NEAR(distance, 3.0f, 0.0001f);
    DVL_EXPECT_NEAR(normal.x, axis.x, 0.0001f);
    DVL_EXPECT_NEAR(normal.y, axis.y, 0.0001f);
    DVL_EXPECT_NEAR(normal.z, 0.0f, 0.0001f);
    return true;
}
