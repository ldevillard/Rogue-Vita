#include "dvl/anim/anim.h"

namespace dvl
{
    void LocalToWorld(const Skeleton& skeleton, const Transform* localPose, const Mat4& rootTransform, Mat4* outWorldMatrices)
    {
        for (int i = 0; i < skeleton.boneCount; i++)
        {
            const Mat4 localTransform = Mat4::FromTransform(localPose[i]);
            const std::int16_t parentIndex = skeleton.parents[i];

            if (parentIndex < 0)
            {
                outWorldMatrices[i] = rootTransform * localTransform;
            }
            else
            {
                outWorldMatrices[i] = outWorldMatrices[parentIndex] * localTransform;
            }
        }
    }

    void ComputeSkinningMatrices(const Skeleton& skeleton, const Mat4* worldPose, Mat4* outSkinningMatrices)
    {
        for (int i = 0; i < skeleton.boneCount; i++)
        {
            outSkinningMatrices[i] = worldPose[i] * skeleton.inverseBindMatrices[i];
        }
    }

    void Evaluate(const Animation& animation, float time, Transform* outPose)
    {
        const float frameFloat = time * animation.fps;

        int frameA = static_cast<int>(frameFloat);

        const float alpha = frameFloat - static_cast<float>(frameA);

        int frameB = frameA + 1;

        // End of clip
        if (frameB >= animation.frameCount)
            frameB = animation.frameCount - 1;

        const Transform* poseA = animation.keyframes + frameA * animation.boneCount;
        const Transform* poseB = animation.keyframes + frameB * animation.boneCount;

        for (int i = 0; i < animation.boneCount; i++)
        {
            outPose[i].rotation = Slerp(poseA[i].rotation, poseB[i].rotation, alpha);

            outPose[i].translation = Lerp(poseA[i].translation, poseB[i].translation, alpha);

            outPose[i].scale = Lerp(poseA[i].scale, poseB[i].scale, alpha);
        }
    }
}