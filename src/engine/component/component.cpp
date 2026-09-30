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

std::unique_ptr<Component> Component::CreateEmpty(Entity&) const
{
    dvl::Log(dvl::LogLevel::Error, "Component does not support Instantiate");

    return nullptr;
}

void Component::CopyFrom(const Component&, const ReferenceMapping&)
{
}

std::tuple<> Component::Fields()
{
    return std::tie();
}

std::tuple<> Component::Fields() const
{
    return std::tie();
}

const Entity* Component::GetEntity() const
{
    return &entity;
}

const Transform& Component::GetTransform() const
{
    return entity.transform;
}

World& Component::GetWorld() const
{
    return entity.GetWorld();
}