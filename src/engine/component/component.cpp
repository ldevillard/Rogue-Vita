#include "engine/component/component.h"

const Entity* Component::GetEntity() const
{
    return entity;
}

void Component::SetEntity(Entity* entity)
{
    this->entity = entity;
}