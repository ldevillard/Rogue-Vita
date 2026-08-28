#include "../unit_test.h"

#include "dvl/anim/anim.h"

namespace
{
    constexpr float Epsilon = 0.0001f;
    constexpr float Pi = 3.14159265358979323846f;
}

DVL_TEST(LocalToWorldDoesNothingForAnEmptySkeleton)
{
    const dvl::Skeleton skeleton;
    dvl::Mat4 output = dvl::Mat4::Translation(dvl::Vec3(1.0f, 2.0f, 3.0f));

    dvl::LocalToWorld(skeleton, nullptr, dvl::Mat4::Identity(), &output);

    const dvl::Vec4 position = output * dvl::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    DVL_EXPECT_EQ(position.x, 1.0f);
    DVL_EXPECT_EQ(position.y, 2.0f);
    DVL_EXPECT_EQ(position.z, 3.0f);
    DVL_EXPECT_EQ(position.w, 1.0f);

    return true;
}

DVL_TEST(LocalToWorldCombinesRootAndLocalTransform)
{
    const std::int16_t parents[] = { -1 };
    const dvl::Skeleton skeleton { 1, parents };

    dvl::Transform localPose[1];
    localPose[0].rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), Pi / 2.0f);
    localPose[0].translation = dvl::Vec4(1.0f, 2.0f, 3.0f, 0.0f);
    localPose[0].scale = dvl::Vec4(2.0f, 3.0f, 4.0f, 0.0f);

    const dvl::Mat4 rootTransform = dvl::Mat4::Translation(dvl::Vec3(10.0f, 20.0f, 30.0f));
    dvl::Mat4 worldMatrices[1];

    dvl::LocalToWorld(skeleton, localPose, rootTransform, worldMatrices);

    const dvl::Vec4 transformed = worldMatrices[0] * dvl::Vec4(1.0f, 0.0f, 0.0f, 1.0f);
    DVL_EXPECT_NEAR(transformed.x, 11.0f, Epsilon);
    DVL_EXPECT_NEAR(transformed.y, 24.0f, Epsilon);
    DVL_EXPECT_NEAR(transformed.z, 33.0f, Epsilon);
    DVL_EXPECT_NEAR(transformed.w, 1.0f, Epsilon);

    return true;
}

DVL_TEST(LocalToWorldAccumulatesParentTransforms)
{
    const std::int16_t parents[] = { -1, 0, 1 };
    const dvl::Skeleton skeleton { 3, parents };

    dvl::Transform localPose[3];
    localPose[0].rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), Pi / 2.0f);
    localPose[0].translation = dvl::Vec4(1.0f, 0.0f, 0.0f, 0.0f);
    localPose[1].translation = dvl::Vec4(2.0f, 0.0f, 0.0f, 0.0f);
    localPose[2].translation = dvl::Vec4(0.0f, 3.0f, 0.0f, 0.0f);

    const dvl::Mat4 rootTransform = dvl::Mat4::Translation(dvl::Vec3(10.0f, 20.0f, 30.0f));
    dvl::Mat4 worldMatrices[3];

    dvl::LocalToWorld(skeleton, localPose, rootTransform, worldMatrices);

    const dvl::Vec4 origin(0.0f, 0.0f, 0.0f, 1.0f);
    const dvl::Vec4 rootPosition = worldMatrices[0] * origin;
    const dvl::Vec4 childPosition = worldMatrices[1] * origin;
    const dvl::Vec4 grandchildPosition = worldMatrices[2] * origin;

    DVL_EXPECT_NEAR(rootPosition.x, 11.0f, Epsilon);
    DVL_EXPECT_NEAR(rootPosition.y, 20.0f, Epsilon);
    DVL_EXPECT_NEAR(rootPosition.z, 30.0f, Epsilon);

    DVL_EXPECT_NEAR(childPosition.x, 11.0f, Epsilon);
    DVL_EXPECT_NEAR(childPosition.y, 22.0f, Epsilon);
    DVL_EXPECT_NEAR(childPosition.z, 30.0f, Epsilon);

    DVL_EXPECT_NEAR(grandchildPosition.x, 8.0f, Epsilon);
    DVL_EXPECT_NEAR(grandchildPosition.y, 22.0f, Epsilon);
    DVL_EXPECT_NEAR(grandchildPosition.z, 30.0f, Epsilon);

    return true;
}

DVL_TEST(LocalToWorldAppliesRootTransformToEverySkeletonRoot)
{
    const std::int16_t parents[] = { -1, -1, 0 };
    const dvl::Skeleton skeleton { 3, parents };

    dvl::Transform localPose[3];
    localPose[0].translation = dvl::Vec4(1.0f, 0.0f, 0.0f, 0.0f);
    localPose[1].translation = dvl::Vec4(0.0f, 2.0f, 0.0f, 0.0f);
    localPose[2].translation = dvl::Vec4(0.0f, 0.0f, 3.0f, 0.0f);

    const dvl::Mat4 rootTransform = dvl::Mat4::Translation(dvl::Vec3(-5.0f, 4.0f, 2.0f));
    dvl::Mat4 worldMatrices[3];

    dvl::LocalToWorld(skeleton, localPose, rootTransform, worldMatrices);

    const dvl::Vec4 origin(0.0f, 0.0f, 0.0f, 1.0f);
    const dvl::Vec4 firstRoot = worldMatrices[0] * origin;
    const dvl::Vec4 secondRoot = worldMatrices[1] * origin;
    const dvl::Vec4 child = worldMatrices[2] * origin;

    DVL_EXPECT_NEAR(firstRoot.x, -4.0f, Epsilon);
    DVL_EXPECT_NEAR(firstRoot.y, 4.0f, Epsilon);
    DVL_EXPECT_NEAR(firstRoot.z, 2.0f, Epsilon);

    DVL_EXPECT_NEAR(secondRoot.x, -5.0f, Epsilon);
    DVL_EXPECT_NEAR(secondRoot.y, 6.0f, Epsilon);
    DVL_EXPECT_NEAR(secondRoot.z, 2.0f, Epsilon);

    DVL_EXPECT_NEAR(child.x, -4.0f, Epsilon);
    DVL_EXPECT_NEAR(child.y, 4.0f, Epsilon);
    DVL_EXPECT_NEAR(child.z, 5.0f, Epsilon);

    return true;
}
