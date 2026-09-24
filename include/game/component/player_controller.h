#pragma once

#include <dvl/tween/tween.h>

#include "engine/component/animator.h"
#include "engine/component/behavior.h"

class Camera;

struct PlayerAnimation
{
    AnimationClip idle;
    AnimationClip run;
};

class PlayerController : public Behavior
{
public:
    PlayerController(Entity& entity, const Camera& camera, Animator& animator, const PlayerAnimation& animations);
    ~PlayerController() override;

    void Update(float deltaTime) override;

    float moveSpeed = 5.0f;
    float dashDistance = 3.0f;
    float dashDuration = 0.15f;
    float animationTransitionDuration = 0.1f;

private:
    void Dash();

    const Camera& _camera;
    
    Animator& _animator;
    PlayerAnimation _animations;

    dvl::ITween* _dashTween = nullptr;
};
