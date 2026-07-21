#pragma once

#include <glm/glm.hpp>

#include "engine/component/behavior.h"

class Transform;

class SpringArm : public Behavior
{
public:
    SpringArm(Entity& entity, const Transform& target);

    void Update(float deltaTime) override;

    float movementSpeed = 3;

private:
    const Transform& _target;

    glm::vec3 _targetOffset;
};
