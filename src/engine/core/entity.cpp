#include "engine/core/entity.h"

#include <dvl/log/log.h>

#include "engine/core/entity_ref.h"

Entity::Entity(World& world)
    : _world(world)
{
}

Entity::~Entity()
{
    for (std::unique_ptr<Component>& component : _components)
    {
        _world.UnRegisterComponent(component.get());
    }
}

World& Entity::GetWorld() const
{
    return _world;
}

EntityRef Entity::ToRef() const
{
    return EntityRef { id };
}

Component* Entity::AttachComponent(std::unique_ptr<Component> component)
{
    if (component == nullptr)
        return nullptr;

    if (component->GetEntity() != this)
    {
        dvl::Log(dvl::LogLevel::Error, "Cannot attach a component owned by another entity");
        return nullptr;
    }

    Component* result = component.get();

    _components.push_back(std::move(component));
    _world.RegisterComponent(result);

    return result;
}

const std::vector<std::unique_ptr<Component>>& Entity::GetComponents() const
{
    return _components;
}