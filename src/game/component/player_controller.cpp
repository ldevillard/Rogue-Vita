#include "game/component/player_controller.h"

#include <dvl/input/input.h>
#include <dvl/tween/tweener.h>

#include <glm/glm.hpp>

#include "engine/component/camera.h"
#include "engine/core/entity.h"

PlayerController::PlayerController(Entity& entity, const Camera& camera)
    : Behavior(entity), _camera(camera)
{
}

PlayerController::~PlayerController()
{
    if (_dashTween != nullptr)
        _dashTween->Kill();
}

void PlayerController::Update(float deltaTime)
{
    if (_dashTween != nullptr)
        return;

    const dvl::StickState& stick = dvl::Input::GetState().leftStick;

    glm::vec2 input{stick.x, stick.y};
    // Avoid moving faster diagonally
    input /= std::max(1.0f, glm::length(input));

    // Projection on XZ plane
    glm::vec3 forward = _camera.GetEntity()->transform.GetForward();
    forward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));

    // Projection on XZ plane
    glm::vec3 right = _camera.GetEntity()->transform.GetRight();
    right = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

    const glm::vec3 movement = right * input.x + forward * input.y;

    if (glm::dot(movement, movement) == 0.0f)
        return;

    entity.transform.LookDirection(movement);

    if (dvl::Input::IsButtonDown(dvl::GamepadButton::Circle))
    {
        Dash();
        return;
    }

    entity.transform.position += movement * moveSpeed * deltaTime;
}

void PlayerController::Dash()
{
    const glm::vec3 from = entity.transform.position;

    // TODO: Raycast in the dash direction and reduce the distance when an obstacle is hit
    const glm::vec3 to = from + entity.transform.GetForward() * dashDistance;

    dvl::Tween<glm::vec3>& tween = dvl::Tweener::Create(from, to, dashDuration, dvl::Easing::OutSine);
    _dashTween = &tween;

    tween.OnUpdate([this](const glm::vec3& position)
    {
        entity.transform.position = position;
    });

    tween.OnComplete([this]()
    {
        _dashTween = nullptr;
    });
}
