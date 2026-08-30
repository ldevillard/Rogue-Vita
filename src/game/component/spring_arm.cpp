#include "game/component/spring_arm.h"

#include <dvl/math/math.h>

#include "engine/core/entity.h"
#include "engine/core/transform.h"

SpringArm::SpringArm(Entity& entity, const Transform& target)
    : Behavior(entity), _target(target)
{
    const dvl::Vec3 forward = entity.transform.GetForward();
    const dvl::Vec3 toTarget = target.position - entity.transform.position;

    const float springLength = dvl::Dot(toTarget, forward);

    _targetOffset = forward * -springLength;
}

void SpringArm::Update(float deltaTime)
{
    const float t = dvl::Clamp(deltaTime * movementSpeed, 0.0f, 1.0f);
    entity.transform.position = dvl::Lerp(entity.transform.position, _target.position + _targetOffset, t);
}
