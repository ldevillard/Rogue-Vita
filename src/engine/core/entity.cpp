#include "engine/core/entity.h"

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

World& Entity::GetWorld()
{
    return _world;
}

EntityRef Entity::ToRef() const
{
    return EntityRef { id };
}

const std::vector<std::unique_ptr<Component>>& Entity::GetComponents() const
{
    return _components;
}