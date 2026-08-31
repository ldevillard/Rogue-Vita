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

DVL_TEST(ComputeSkinningMatricesDoesNothingForAnEmptySkeleton)
{
    const dvl::Skeleton skeleton;
    dvl::Mat4 output = dvl::Mat4::Translation(dvl::Vec3(1.0f, 2.0f, 3.0f));

    dvl::ComputeSkinningMatrices(skeleton, nullptr, &output);

    const dvl::Vec4 position = output * dvl::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    DVL_EXPECT_EQ(position.x, 1.0f);
    DVL_EXPECT_EQ(position.y, 2.0f);
    DVL_EXPECT_EQ(position.z, 3.0f);
    DVL_EXPECT_EQ(position.w, 1.0f);

    return true;
}

DVL_TEST(ComputeSkinningMatricesCancelsTheBindPose)
{
    const dvl::Mat4 translation = dvl::Mat4::Translation(dvl::Vec3(3.0f, -2.0f, 5.0f));
    const dvl::Mat4 inverseTranslation = dvl::Mat4::Translation(dvl::Vec3(-3.0f, 2.0f, -5.0f));

    const dvl::Mat4 rotation = dvl::Mat4::Rotation(
        dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), Pi / 2.0f));
    const dvl::Mat4 inverseRotation = dvl::Mat4::Rotation(
        dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), -Pi / 2.0f));

    const dvl::Mat4 scale = dvl::Mat4::Scale(dvl::Vec3(2.0f, 4.0f, 0.5f));
    const dvl::Mat4 inverseScale = dvl::Mat4::Scale(dvl::Vec3(0.5f, 0.25f, 2.0f));

    const dvl::Mat4 worldPose[] =
    {
        translation,
        translation * rotation * scale
    };

    const dvl::Mat4 inverseBindMatrices[] =
    {
        inverseTranslation,
        inverseScale * inverseRotation * inverseTranslation
    };

    const dvl::Skeleton skeleton { 2, nullptr, inverseBindMatrices };
    dvl::Mat4 skinningMatrices[2];

    dvl::ComputeSkinningMatrices(skeleton, worldPose, skinningMatrices);

    for (int boneIndex = 0; boneIndex < skeleton.boneCount; boneIndex++)
    {
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                const float expected = column == row ? 1.0f : 0.0f;
                DVL_EXPECT_NEAR(skinningMatrices[boneIndex].m[column][row], expected, Epsilon);
            }
        }
    }

    return true;
}

DVL_TEST(ComputeSkinningMatricesTransformsEachBoneFromBindPoseToWorldPose)
{
    const dvl::Mat4 worldPose[] =
    {
        dvl::Mat4::Translation(dvl::Vec3(2.0f, 3.0f, 4.0f)),
        dvl::Mat4::Translation(dvl::Vec3(0.0f, 1.0f, 0.0f)) *
            dvl::Mat4::Rotation(
                dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), Pi / 2.0f))
    };

    const dvl::Mat4 inverseBindMatrices[] =
    {
        dvl::Mat4::Identity(),
        dvl::Mat4::Translation(dvl::Vec3(0.0f, -1.0f, 0.0f))
    };

    const dvl::Skeleton skeleton { 2, nullptr, inverseBindMatrices };
    dvl::Mat4 skinningMatrices[2];

    dvl::ComputeSkinningMatrices(skeleton, worldPose, skinningMatrices);

    const dvl::Vec4 rootResult =
        skinningMatrices[0] * dvl::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    DVL_EXPECT_NEAR(rootResult.x, 2.0f, Epsilon);
    DVL_EXPECT_NEAR(rootResult.y, 3.0f, Epsilon);
    DVL_EXPECT_NEAR(rootResult.z, 4.0f, Epsilon);
    DVL_EXPECT_NEAR(rootResult.w, 1.0f, Epsilon);

    const dvl::Vec4 jointResult =
        skinningMatrices[1] * dvl::Vec4(0.0f, 1.0f, 0.0f, 1.0f);
    DVL_EXPECT_NEAR(jointResult.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(jointResult.y, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(jointResult.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(jointResult.w, 1.0f, Epsilon);

    const dvl::Vec4 tipResult =
        skinningMatrices[1] * dvl::Vec4(0.0f, 2.0f, 0.0f, 1.0f);
    DVL_EXPECT_NEAR(tipResult.x, -1.0f, Epsilon);
    DVL_EXPECT_NEAR(tipResult.y, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(tipResult.z, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(tipResult.w, 1.0f, Epsilon);

    return true;
}

DVL_TEST(EvaluateReturnsFirstFrameAtTimeZero)
{
    dvl::Transform keyframes[2 * 2]; // 2 bones, 2 frames
    
    // Frame 0
    keyframes[0].rotation = dvl::Quat::Identity();
    keyframes[0].translation = dvl::Vec4(1.0f, 2.0f, 3.0f, 0.0f);
    keyframes[0].scale = dvl::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    
    keyframes[1].rotation = dvl::Quat::Identity();
    keyframes[1].translation = dvl::Vec4(4.0f, 5.0f, 6.0f, 0.0f);
    keyframes[1].scale = dvl::Vec4(2.0f, 2.0f, 2.0f, 0.0f);
    
    // Frame 1
    keyframes[2].rotation = dvl::Quat::Identity();
    keyframes[2].translation = dvl::Vec4(10.0f, 20.0f, 30.0f, 0.0f);
    keyframes[2].scale = dvl::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    
    keyframes[3].rotation = dvl::Quat::Identity();
    keyframes[3].translation = dvl::Vec4(40.0f, 50.0f, 60.0f, 0.0f);
    keyframes[3].scale = dvl::Vec4(2.0f, 2.0f, 2.0f, 0.0f);
    
    dvl::Animation animation;
    animation.fps = 30.0f;
    animation.boneCount = 2;
    animation.frameCount = 2;
    animation.keyframes = keyframes;
    
    dvl::Transform outPose[2];
    dvl::Evaluate(animation, 0.0f, outPose);
    
    // Check first bone
    DVL_EXPECT_NEAR(outPose[0].translation.x, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].translation.y, 2.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].translation.z, 3.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].scale.x, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].scale.y, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].scale.z, 1.0f, Epsilon);
    
    // Check second bone
    DVL_EXPECT_NEAR(outPose[1].translation.x, 4.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[1].translation.y, 5.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[1].translation.z, 6.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[1].scale.x, 2.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[1].scale.y, 2.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[1].scale.z, 2.0f, Epsilon);
    
    return true;
}

DVL_TEST(EvaluateReturnsLastFrameAtEndTime)
{
    dvl::Transform keyframes[2 * 2]; // 2 bones, 2 frames
    
    // Frame 0
    keyframes[0].rotation = dvl::Quat::Identity();
    keyframes[0].translation = dvl::Vec4(1.0f, 2.0f, 3.0f, 0.0f);
    keyframes[0].scale = dvl::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    
    keyframes[1].rotation = dvl::Quat::Identity();
    keyframes[1].translation = dvl::Vec4(4.0f, 5.0f, 6.0f, 0.0f);
    keyframes[1].scale = dvl::Vec4(2.0f, 2.0f, 2.0f, 0.0f);
    
    // Frame 1
    keyframes[2].rotation = dvl::Quat::Identity();
    keyframes[2].translation = dvl::Vec4(10.0f, 20.0f, 30.0f, 0.0f);
    keyframes[2].scale = dvl::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    
    keyframes[3].rotation = dvl::Quat::Identity();
    keyframes[3].translation = dvl::Vec4(40.0f, 50.0f, 60.0f, 0.0f);
    keyframes[3].scale = dvl::Vec4(2.0f, 2.0f, 2.0f, 0.0f);
    
    dvl::Animation animation;
    animation.fps = 30.0f;
    animation.boneCount = 2;
    animation.frameCount = 2;
    animation.duration = 1.0f / 30.0f;
    animation.keyframes = keyframes;
    
    dvl::Transform outPose[2];
    dvl::Evaluate(animation, 1.0f / 30.0f, outPose);
    
    // Check first bone (should return frame 1)
    DVL_EXPECT_NEAR(outPose[0].translation.x, 10.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].translation.y, 20.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].translation.z, 30.0f, Epsilon);
    
    // Check second bone
    DVL_EXPECT_NEAR(outPose[1].translation.x, 40.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[1].translation.y, 50.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[1].translation.z, 60.0f, Epsilon);
    
    return true;
}

DVL_TEST(EvaluateInterpolatesBetweenFrames)
{
    dvl::Transform keyframes[1 * 3]; // 1 bone, 3 frames
    
    // Frame 0
    keyframes[0].rotation = dvl::Quat::Identity();
    keyframes[0].translation = dvl::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
    keyframes[0].scale = dvl::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    
    // Frame 1
    keyframes[1].rotation = dvl::Quat::Identity();
    keyframes[1].translation = dvl::Vec4(10.0f, 0.0f, 0.0f, 0.0f);
    keyframes[1].scale = dvl::Vec4(2.0f, 2.0f, 2.0f, 0.0f);
    
    // Frame 2
    keyframes[2].rotation = dvl::Quat::Identity();
    keyframes[2].translation = dvl::Vec4(20.0f, 0.0f, 0.0f, 0.0f);
    keyframes[2].scale = dvl::Vec4(3.0f, 3.0f, 3.0f, 0.0f);
    
    dvl::Animation animation;
    animation.fps = 30.0f;
    animation.boneCount = 1;
    animation.frameCount = 3;
    animation.keyframes = keyframes;
    
    // Evaluate at time = 1/60 seconds: frameFloat = (1/60) * 30 = 0.5
    // frameA = 0, alpha = 0.5, frameB = 1 (halfway between frame 0 and frame 1)
    dvl::Transform outPose[1];
    dvl::Evaluate(animation, 1.0f / 60.0f, outPose);
    
    // Interpolated translation should be (0.0 + 10.0) / 2 = 5.0
    // Interpolated scale should be (1.0 + 2.0) / 2 = 1.5
    DVL_EXPECT_NEAR(outPose[0].translation.x, 5.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].translation.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].scale.x, 1.5f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].scale.y, 1.5f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].scale.z, 1.5f, Epsilon);
    
    return true;
}

DVL_TEST(EvaluateHandlesRotationInterpolation)
{
    dvl::Transform keyframes[1 * 2]; // 1 bone, 2 frames
    
    // Frame 0: No rotation
    keyframes[0].rotation = dvl::Quat::Identity();
    keyframes[0].translation = dvl::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
    keyframes[0].scale = dvl::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    
    // Frame 1: 90 degree rotation around Z axis
    keyframes[1].rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), Pi / 2.0f);
    keyframes[1].translation = dvl::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
    keyframes[1].scale = dvl::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    
    dvl::Animation animation;
    animation.fps = 30.0f;
    animation.boneCount = 1;
    animation.frameCount = 2;
    animation.keyframes = keyframes;
    
    // Evaluate at halfway between frames (alpha = 0.5)
    dvl::Transform outPose[1];
    dvl::Evaluate(animation, 1.0f / 60.0f, outPose);
    
    // The quaternion should be interpolated via Slerp
    // At 45 degrees, sin(angle) should be sqrt(2)/2
    const float expectedWComponent = std::cos(Pi / 8.0f);
    const float expectedZComponent = std::sin(Pi / 8.0f);
    
    DVL_EXPECT_NEAR(outPose[0].rotation.w, expectedWComponent, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].rotation.z, expectedZComponent, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].rotation.x, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].rotation.y, 0.0f, Epsilon);
    
    return true;
}

DVL_TEST(EvaluateClampsBeyondLastFrame)
{
    dvl::Transform keyframes[1 * 5]; // 1 bone, 5 frames
    
    // Setup frames with different translations
    for (int i = 0; i < 5; i++)
    {
        keyframes[i].rotation = dvl::Quat::Identity();
        keyframes[i].translation = dvl::Vec4(static_cast<float>(i * 10), 0.0f, 0.0f, 0.0f);
        keyframes[i].scale = dvl::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    }
    
    dvl::Animation animation;
    animation.fps = 30.0f;
    animation.boneCount = 1;
    animation.frameCount = 5;
    animation.keyframes = keyframes;
    
    // Evaluate at time that gives frameA = 3 (valid), frameB = 4 -> clamped to 4
    // frameFloat = (3/30) * 30 = 3, frameA = 3, frameB = 4 (within bounds, no clamping needed)
    // But let's test the slightly later time where frameB would be clamped
    dvl::Transform outPose[1];
    dvl::Evaluate(animation, 4.1f / 30.0f, outPose);
    
    // frameFloat = (4.1/30) * 30 = 4.1, frameA = 4, alpha = 0.1, frameB = 5 -> clamped to 4
    // So we interpolate between frame 4 and frame 4 with alpha = 0.1
    // Frame 4 has translation (40, 0, 0)
    DVL_EXPECT_NEAR(outPose[0].translation.x, 40.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].translation.y, 0.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[0].scale.x, 1.0f, Epsilon);
    
    return true;
}

DVL_TEST(EvaluateHandlesMultipleBonesCorrectly)
{
    dvl::Transform keyframes[3 * 2]; // 3 bones, 2 frames
    
    // Frame 0
    keyframes[0].rotation = dvl::Quat::Identity();
    keyframes[0].translation = dvl::Vec4(1.0f, 0.0f, 0.0f, 0.0f);
    keyframes[0].scale = dvl::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    
    keyframes[1].rotation = dvl::Quat::Identity();
    keyframes[1].translation = dvl::Vec4(0.0f, 2.0f, 0.0f, 0.0f);
    keyframes[1].scale = dvl::Vec4(2.0f, 2.0f, 2.0f, 0.0f);
    
    keyframes[2].rotation = dvl::Quat::Identity();
    keyframes[2].translation = dvl::Vec4(0.0f, 0.0f, 3.0f, 0.0f);
    keyframes[2].scale = dvl::Vec4(3.0f, 3.0f, 3.0f, 0.0f);
    
    // Frame 1
    keyframes[3].rotation = dvl::Quat::Identity();
    keyframes[3].translation = dvl::Vec4(10.0f, 0.0f, 0.0f, 0.0f);
    keyframes[3].scale = dvl::Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    
    keyframes[4].rotation = dvl::Quat::Identity();
    keyframes[4].translation = dvl::Vec4(0.0f, 20.0f, 0.0f, 0.0f);
    keyframes[4].scale = dvl::Vec4(2.0f, 2.0f, 2.0f, 0.0f);
    
    keyframes[5].rotation = dvl::Quat::Identity();
    keyframes[5].translation = dvl::Vec4(0.0f, 0.0f, 30.0f, 0.0f);
    keyframes[5].scale = dvl::Vec4(3.0f, 3.0f, 3.0f, 0.0f);
    
    dvl::Animation animation;
    animation.fps = 30.0f;
    animation.boneCount = 3;
    animation.frameCount = 2;
    animation.keyframes = keyframes;
    
    // Evaluate at frame 0
    dvl::Transform outPose[3];
    dvl::Evaluate(animation, 0.0f, outPose);
    
    // Check all three bones
    DVL_EXPECT_NEAR(outPose[0].translation.x, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[1].translation.y, 2.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[2].translation.z, 3.0f, Epsilon);
    
    DVL_EXPECT_NEAR(outPose[0].scale.x, 1.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[1].scale.x, 2.0f, Epsilon);
    DVL_EXPECT_NEAR(outPose[2].scale.x, 3.0f, Epsilon);
    
    return true;
}
