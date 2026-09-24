#pragma once

#include <vector>

#include "engine/component/component.h"
#include "engine/render/animation.h"
#include "engine/render/skeleton.h"

enum class AnimationLoopMode
{
    Loop,
    Once,
};

struct AnimationClip
{
    AnimationHandle animation;
    AnimationLoopMode loopMode = AnimationLoopMode::Loop;
    float speed = 1.0f;
};

struct AnimationState
{
    AnimationClip clip;
    float time = 0.0f;
    bool finished = false;
};

class Animator : public Component
{
public:
    Animator(Entity& entity, const SkeletonHandle& skeletonHandle, const AnimationHandle& animationHandle);

    void Update(float deltaTime, const Skeleton& skeleton, const Animation& currentAnimation, const Animation* nextAnimation = nullptr);

    bool IsValid() const;

    void Play(const AnimationClip& clip, float transitionDuration = 0.0f);
    void Stop();

    bool IsPlaying() const;
    bool IsTransitioning() const;
    bool IsFinished() const;

    const SkeletonHandle& GetSkeletonHandle() const;
    const AnimationHandle& GetCurrentAnimationHandle() const;
    const AnimationHandle& GetNextAnimationHandle() const;
    const dvl::Mat4* GetSkinningMatrices() const;
    int GetBoneCount() const;

private:
    void UpdateState(AnimationState& state, const Animation& animation, float deltaTime);

    SkeletonHandle _skeletonHandle;
    int _boneCount = 0;

    AnimationState _current;
    AnimationState _next;

    bool _transitioning = false;
    float _transitionTime = 0.0f;
    float _transitionDuration = 0.0f;

    std::vector<dvl::Transform> _currentPose;
    std::vector<dvl::Transform> _nextPose;
    std::vector<dvl::Transform> _pose;

    std::vector<dvl::Mat4> _skinningMatrices;
};
