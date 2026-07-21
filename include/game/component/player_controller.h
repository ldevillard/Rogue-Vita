#pragma once

#include "engine/component/behavior.h"

class Camera;

class PlayerController : public Behavior
{
public:
    PlayerController(Entity& entity, const Camera& camera);

    void Update(float deltaTime) override;

    float moveSpeed = 3.0f;

private:
    const Camera& _camera;
};
