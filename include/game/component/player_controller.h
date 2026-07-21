#pragma once

#include "engine/component/component.h"

class Camera;

class PlayerController : public Component
{
public:
    PlayerController(Entity& entity, const Camera& camera);

    void Update(float deltaTime);

    float moveSpeed = 3.0f;

private:
    const Camera& _camera;
};
