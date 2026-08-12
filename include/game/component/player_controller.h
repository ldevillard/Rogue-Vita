#pragma once

#include <dvl/tween/tween.h>

#include "engine/component/behavior.h"

class Camera;

class PlayerController : public Behavior
{
public:
    PlayerController(Entity& entity, const Camera& camera);
    ~PlayerController() override;

    void Update(float deltaTime) override;

    float moveSpeed = 8.0f;
    float dashDistance = 4.0f;
    float dashDuration = 0.15f;

private:
    void Dash();

    const Camera& _camera;
    dvl::ITween* _dashTween = nullptr;
};
