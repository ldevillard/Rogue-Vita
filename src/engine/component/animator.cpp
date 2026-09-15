#include "engine/component/animator.h"

#include <dvl/anim/anim.h>
#include <cmath>

Animator::Animator(Entity& entity, const SkeletonHandle& skeletonHandle, const AnimationHandle& animationHandle)
    : Component(entity), _skeletonHandle(skeletonHandle), _animationHandle(animationHandle)
{
}

void Animator::Update(float deltaTime, const Skeleton& skeleton, const Animation& animation)
{
    if (!skeleton.IsValid() || !animation.IsValid() || skeleton.boneCount != animation.boneCount)
    {
        Clear();
        return;
    }

    if (_boneCount != skeleton.boneCount)
    {
        _boneCount = skeleton.boneCount;
        _pose.resize(_boneCount);
        _skinningMatrices.resize(_boneCount);
    }

    _time = std::fmod(_time + deltaTime, animation.duration);

    dvl::Evaluate(animation.GetView(), _time, _pose.data());
    dvl::LocalToWorld(skeleton.GetView(), _pose.data(), dvl::Mat4::Identity(), _skinningMatrices.data());
    dvl::ComputeSkinningMatrices(skeleton.GetView(), _skinningMatrices.data(), _skinningMatrices.data());
}

void Animator::Clear()
{
    _time = 0.0f;
    _boneCount = 0;
    _pose.clear();
    _skinningMatrices.clear();
}

bool Animator::IsValid() const
{
    return _boneCount > 0 && _pose.size() == static_cast<std::size_t>(_boneCount) && _skinningMatrices.size() == static_cast<std::size_t>(_boneCount);
}

const SkeletonHandle& Animator::GetSkeletonHandle() const
{
    return _skeletonHandle;
}

const AnimationHandle& Animator::GetAnimationHandle() const
{
    return _animationHandle;
}

const dvl::Mat4* Animator::GetSkinningMatrices() const
{
    return IsValid() ? _skinningMatrices.data() : nullptr;
}

int Animator::GetBoneCount() const
{
    return _boneCount;
}
