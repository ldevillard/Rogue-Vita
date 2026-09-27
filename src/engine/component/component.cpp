#include "engine/component/component.h"

#include "engine/core/entity.h"
#include "engine/core/transform.h"

Component::Component(Entity& entity)
    : entity(entity)
{
}

const Entity* Component::GetEntity() const
{
    return &entity;
}

const Transform& Component::GetTransform() const
{
    return entity.transform;
}
