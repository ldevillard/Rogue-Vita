#include "engine/core/entity.h"

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

const std::vector<std::unique_ptr<Component>>& Entity::GetComponents() const
{
    return _components;
}