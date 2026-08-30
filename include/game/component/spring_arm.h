#pragma once

#include <dvl/math/vec.h>

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

    dvl::Vec3 _targetOffset;
};
