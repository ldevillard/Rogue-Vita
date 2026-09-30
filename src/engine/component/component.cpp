#include "engine/component/component.h"

#include <dvl/log/log.h>

#include "engine/core/entity.h"
#include "engine/core/transform.h"

Component::Component(Entity& entity)
    : entity(entity)
{
}

void Component::Start()
{
    // void Start SHOULDN'T Add/Remove components or Entity deletion for nwow
}

const Entity* Component::GetEntity() const
{
    return &entity;
}

const Transform& Component::GetTransform() const
{
    return entity.transform;
}
