#pragma once

#include <vector>

#include "engine/component/component.h"
#include "engine/render/animation.h"
#include "engine/render/skeleton.h"

class Animator : public Component
{
public:
    Animator(Entity& entity, const SkeletonHandle& skeletonHandle, const AnimationHandle& animationHandle);

    void Update(float deltaTime, const Skeleton& skeleton, const Animation& animation);
    void Clear();

    bool IsValid() const;
    const SkeletonHandle& GetSkeletonHandle() const;
    const AnimationHandle& GetAnimationHandle() const;
    const dvl::Mat4* GetSkinningMatrices() const;
    int GetBoneCount() const;

private:
    SkeletonHandle _skeletonHandle;
    AnimationHandle _animationHandle;

    float _time = 0.0f;
    int _boneCount = 0;

    std::vector<dvl::Transform> _pose;
    std::vector<dvl::Mat4> _skinningMatrices;
};
