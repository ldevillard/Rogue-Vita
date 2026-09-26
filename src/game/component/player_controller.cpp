#include "game/component/player_controller.h"

#include <cmath>

#include <dvl/input/input.h>
#include <dvl/math/math.h>
#include <dvl/tween/tweener.h>

#include "engine/component/camera.h"
#include "engine/core/entity.h"
#include "engine/debug/debug_draw.h"

PlayerController::PlayerController(Entity& entity, const Camera& camera, Animator& animator, const PlayerAnimation& animations)
    : Behavior(entity), _camera(camera), _animator(animator), _animations(animations)
{
}

PlayerController::~PlayerController()
{
    if (_dashTween != nullptr)
        _dashTween->Kill();
}

void PlayerController::Update(float deltaTime)
{
    drawDebugFov();

    if (_dashTween != nullptr)
        return;

    const dvl::StickState& stick = dvl::Input::GetState().leftStick;

    dvl::Vec2 input{stick.x, stick.y};
    // Avoid moving faster diagonally
    input = input / std::max(1.0f, input.Length());

    // Projection on XZ plane
    dvl::Vec3 forward = _camera.GetEntity()->transform.GetForward();
    forward = dvl::Vec3(forward.x, 0.0f, forward.z).Normalized();

    // Projection on XZ plane
    dvl::Vec3 right = _camera.GetEntity()->transform.GetRight();
    right = dvl::Vec3(right.x, 0.0f, right.z).Normalized();

    const dvl::Vec3 movement = right * input.x + forward * input.y;

    if (dvl::Input::IsButtonDown(dvl::GamepadButton::Square))
    {
        _animator.Play(_animations.attack, animationTransitionDuration);
        _isAttacking = true;
    }

    if (_isAttacking)
    {
        if (!_animator.IsFinished())
            return;

        _isAttacking = false;
    }

    if (movement.LengthSquared() == 0.0f)
    {
        _animator.Play(_animations.idle, animationTransitionDuration);
        return;
    }

    _animator.Play(_animations.run, animationTransitionDuration);

    if (dvl::Input::IsButtonDown(dvl::GamepadButton::Circle))
    {
        dash();
        return;
    }
    
    entity.transform.rotation = dvl::Nlerp(entity.transform.rotation, dvl::Quat::LookRotation(movement), rotationSpeed * deltaTime);
    entity.transform.position += movement * moveSpeed * deltaTime;
}

void PlayerController::dash()
{
    const dvl::Vec3 from = entity.transform.position;

    // TODO: Raycast in the dash direction and reduce the distance when an obstacle is hit
    const dvl::Vec3 to = from + entity.transform.GetForward() * dashDistance;

    dvl::Tween<dvl::Vec3>& tween = dvl::Tweener::Create(from, to, dashDuration, dvl::Easing::OutSine);
    _dashTween = &tween;

    tween.OnUpdate([this](const dvl::Vec3& position)
    {
        entity.transform.position = position;
    });

    tween.OnComplete([this]()
    {
        _dashTween = nullptr;
    });
}

void PlayerController::drawDebugFov()
{
    const float detectionRadius = 5.0f;
    const float halfAngle = dvl::Radians(50.0f);

    const dvl::Vec3 origin = entity.transform.position;
    const dvl::Vec3 forward = entity.transform.GetForward();
    const dvl::Vec3 right = entity.transform.GetRight();

    const dvl::Vec3 rightLimit = forward * std::cos(halfAngle) + right * std::sin(halfAngle);

    const dvl::Vec3 leftLimit = forward * std::cos(halfAngle) - right * std::sin(halfAngle);

    const dvl::Vec4 blue(0.0f, 0.0f, 1.0f, 1.0f);
    const dvl::Vec4 green(0.0f, 1.0f, 0.0f, 1.0f);

    DebugDraw::DrawLine(origin, origin + forward * detectionRadius, blue);
    DebugDraw::DrawLine(origin, origin + rightLimit * detectionRadius, green);
    DebugDraw::DrawLine(origin, origin + leftLimit * detectionRadius, green);
}
