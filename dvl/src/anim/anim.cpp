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
}