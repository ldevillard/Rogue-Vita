#pragma once

#include "engine/component/component.h"

class Camera;

class PlayerController : public Component
{
public:
    PlayerController(const Camera& camera);

    bool IsValid() const override;
    void Update(float deltaTime);

    float moveSpeed = 3.0f;

private:
    const Camera& _camera;
};
