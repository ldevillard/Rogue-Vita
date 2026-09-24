#include "engine/component/animator.h"

#include <dvl/anim/anim.h>
#include <dvl/math/math.h>

#include <cmath>

Animator::Animator(Entity& entity, const SkeletonHandle& skeletonHandle, const AnimationHandle& animationHandle)
    : Component(entity), _skeletonHandle(skeletonHandle)
{
    _current.clip.animation = animationHandle;
}

void Animator::Update(float deltaTime, const Skeleton& skeleton, const Animation& currentAnimation, const Animation* nextAnimation)
{
    if (!skeleton.IsValid() || !currentAnimation.IsValid() || skeleton.boneCount != currentAnimation.boneCount)
    {
        Stop();
        return;
    }

    if (_transitioning && (nextAnimation == nullptr || !nextAnimation->IsValid() || skeleton.boneCount != nextAnimation->boneCount))
    {
        // Keep playing the current clip if the transition target cannot be used.
        _next = {};
        _transitioning = false;
        _transitionTime = 0.0f;
        _transitionDuration = 0.0f;

        dvl::Log(dvl::LogLevel::Warning, "Animator transition target is invalid, continuing to play current animation");
    }

    // Update bone count and resize pose and skinning matrices if necessary
    if (_boneCount != skeleton.boneCount)
    {
        _boneCount = skeleton.boneCount;
        _currentPose.resize(_boneCount);
        _nextPose.resize(_boneCount);
        _pose.resize(_boneCount);
        _skinningMatrices.resize(_boneCount);
    }

    UpdateState(_current, currentAnimation, deltaTime);
    dvl::Evaluate(currentAnimation.GetView(), _current.time, _currentPose.data());

    if (_transitioning)
    {
        UpdateState(_next, *nextAnimation, deltaTime);
        dvl::Evaluate(nextAnimation->GetView(), _next.time, _nextPose.data());

        _transitionTime += deltaTime;
        const float blend = dvl::Clamp(_transitionTime / _transitionDuration, 0.0f, 1.0f);
        dvl::Blend(_currentPose.data(), _nextPose.data(), _boneCount, blend, _pose.data());

        if (blend >= 1.0f)
        {
            _current = _next;
            _next = {};
            _currentPose.swap(_nextPose);
            _transitioning = false;
            _transitionTime = 0.0f;
            _transitionDuration = 0.0f;
        }
    }
    else
    {
        _pose = _currentPose;
    }

    dvl::LocalToWorld(skeleton.GetView(), _pose.data(), dvl::Mat4::Identity(), _skinningMatrices.data());
    dvl::ComputeSkinningMatrices(skeleton.GetView(), _skinningMatrices.data(), _skinningMatrices.data());
}

bool Animator::IsValid() const
{
    return _boneCount > 0 && _pose.size() == static_cast<std::size_t>(_boneCount) && _skinningMatrices.size() == static_cast<std::size_t>(_boneCount);
}

void Animator::Play(const AnimationClip& clip, float transitionDuration)
{
    if (!clip.animation.IsValid())
        return;

    if (!IsPlaying() || transitionDuration <= 0.0f)
    {
        _current = {clip, 0.0f, false};
        _next = {};
        _transitioning = false;
        _transitionTime = 0.0f;
        _transitionDuration = 0.0f;
        return;
    }

    if (!_transitioning && _current.clip.animation == clip.animation)
        return;

    if (_transitioning && _next.clip.animation == clip.animation)
        return;

    _next = {clip, 0.0f, false};
    _transitioning = true;
    _transitionTime = 0.0f;
    _transitionDuration = transitionDuration;
}

void Animator::Stop()
{
    _current = {};
    _next = {};
    _transitioning = false;
    _transitionTime = 0.0f;
    _transitionDuration = 0.0f;
    _boneCount = 0;
    _currentPose.clear();
    _nextPose.clear();
    _pose.clear();
    _skinningMatrices.clear();
}

bool Animator::IsPlaying() const
{
    return _current.clip.animation.IsValid();
}

bool Animator::IsTransitioning() const
{
    return _transitioning;
}

bool Animator::IsFinished() const
{
    return IsPlaying() && !_transitioning && _current.finished;
}

const SkeletonHandle& Animator::GetSkeletonHandle() const
{
    return _skeletonHandle;
}

const AnimationHandle& Animator::GetCurrentAnimationHandle() const
{
    return _current.clip.animation;
}

const AnimationHandle& Animator::GetNextAnimationHandle() const
{
    return _next.clip.animation;
}

const dvl::Mat4* Animator::GetSkinningMatrices() const
{
    return IsValid() ? _skinningMatrices.data() : nullptr;
}

int Animator::GetBoneCount() const
{
    return _boneCount;
}

void Animator::UpdateState(AnimationState& state, const Animation& animation, float deltaTime)
{
    state.time += deltaTime * state.clip.speed;

    if (state.clip.loopMode == AnimationLoopMode::Loop)
    {
        state.time = std::fmod(state.time, animation.duration);
        state.finished = false;

        return;
    }

    if (state.time >= animation.duration)
    {
        // Evaluate expects a time strictly lower than the clip duration.
        state.time = std::nextafter(animation.duration, 0.0f);
        state.finished = true;
    }
    else if (state.time <= 0.0f)
    {
        state.time = 0.0f;
    }
}
