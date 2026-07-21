#pragma once

#include <glm/glm.hpp>

#include "engine/component/component.h"

class Transform;

// TODO: Maybe make an interface IUpdateable or something similar ? (IGameObject ?)
class SpringArm : public Component
{
public:
    SpringArm(Entity& entity, const Transform& target);

    void Update(float deltaTime);

    float movementSpeed = 3;

private:
    const Transform& _target;

    glm::vec3 _targetOffset;
};
