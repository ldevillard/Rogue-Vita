#include "engine/component/component.h"

Component::Component(Entity& entity)
    : entity(entity)
{
}

const Entity* Component::GetEntity() const
{
    return &entity;
}
