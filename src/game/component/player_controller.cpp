#include "game/component/player_controller.h"

#include <dvl/input/input.h>

#include <glm/glm.hpp>

#include "engine/component/camera.h"
#include "engine/core/entity.h"

PlayerController::PlayerController(const Camera& camera)
    : _camera(camera)
{
}

bool PlayerController::IsValid() const
{
    return entity != nullptr && _camera.GetEntity() != nullptr;
}

void PlayerController::Update(float deltaTime)
{
    if (!IsValid())
        return;

    const dvl::StickState& stick = dvl::Input::GetState().leftStick;

    glm::vec2 input{stick.x, stick.y};
    // Avoid moving faster diagonally
    input /= std::max(1.0f, glm::length(input));

    // Projection on xz plane
    glm::vec3 forward = _camera.GetEntity()->transform.GetForward();
    forward = glm::normalize(glm::vec3{forward.x, 0.0f, forward.z});

    // Projection on xz plane
    glm::vec3 right = _camera.GetEntity()->transform.GetRight();
    right = glm::normalize(glm::vec3{right.x, 0.0f, right.z});

    const glm::vec3 movement = right * input.x + forward * input.y;

    entity->transform.position += movement * moveSpeed * deltaTime;

}
