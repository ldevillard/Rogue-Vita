#pragma once

#include <dvl/math/vec.h>

#include "engine/component/behavior.h"
#include "engine/core/entity_ref.h"

class Transform;

class SpringArm : public Behavior
{
public:
    SpringArm(Entity& entity);
    SpringArm(Entity& entity, EntityRef target);
    
    COMPONENT_TYPES(SpringArm, Behavior, Component)
    COMPONENT_FIELDS(Behavior, movementSpeed, _target)

    void Start() override;

    void Update(float deltaTime) override;

    float movementSpeed = 3;

private:
    EntityRef _target;

    dvl::Vec3 _targetOffset = dvl::Vec3::Zero();
};
