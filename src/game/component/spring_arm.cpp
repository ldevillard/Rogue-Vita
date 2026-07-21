#include "game/component/spring_arm.h"

#include "engine/core/entity.h"
#include "engine/core/transform.h"

SpringArm::SpringArm(Entity& entity, const Transform& target)
    : Component(entity), _target(target)
{
    const glm::vec3 forward = entity.transform.GetForward();
    const glm::vec3 toTarget = target.position - entity.transform.position;

    const float springLength = glm::dot(toTarget, forward);

    _targetOffset = -forward * springLength;
}

void SpringArm::Update(float deltaTime)
{
    const float t = glm::clamp(deltaTime * movementSpeed, 0.0f, 1.0f);
    entity.transform.position = glm::mix(entity.transform.position, _target.position + _targetOffset, t);
}
