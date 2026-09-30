#include "game/component/spring_arm.h"

#include <dvl/math/math.h>

#include "engine/core/entity.h"
#include "engine/core/transform.h"

SpringArm::SpringArm(Entity& entity, EntityRef target)
    : Behavior(entity), _target(target)
{
}

void SpringArm::Start()
{
    Entity* target = _target.Get(GetWorld());
    if (target == nullptr)
        return;

    const dvl::Vec3 forward = entity.transform.GetForward();
    const dvl::Vec3 toTarget = target->transform.position - entity.transform.position;

    const float springLength = dvl::Dot(toTarget, forward);

    _targetOffset = forward * -springLength;
}

void SpringArm::Update(float deltaTime)
{
    Entity* target = _target.Get(GetWorld());
    if (target == nullptr)
        return;

    const float t = dvl::Clamp(deltaTime * movementSpeed, 0.0f, 1.0f);
    entity.transform.position = dvl::Lerp(entity.transform.position, target->transform.position + _targetOffset, t);
}
