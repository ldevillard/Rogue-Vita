#include "engine/core/world.h"

#include <algorithm>
#include <dvl/log/log.h>

#include "engine/core/entity.h"

World::World()
{
}

World::~World()
{
    _entities.clear();
}

Entity* World::CreateEntity()
{
    std::unique_ptr<Entity> entity = std::make_unique<Entity>(*this);
    entity->id = _nextId++;

    Entity* result = entity.get();

    _entities.push_back(std::move(entity));

    return result;
}

void World::DestroyEntity(unsigned int id)
{
     for (auto it = _entities.begin(); it != _entities.end(); ++it)
    {
        if ((*it)->id == id)
        {
            _entities.erase(it);
            return;
        }
    }
}

Entity* World::FindEntity(unsigned int id)
{
    for (const std::unique_ptr<Entity>& entity : _entities)
    {
        if (entity->id == id)
            return entity.get();
    }

    return nullptr;
}

const Entity* World::FindEntity(unsigned int id) const
{
    for (const std::unique_ptr<Entity>& entity : _entities)
    {
        if (entity->id == id)
            return entity.get();
    }

    return nullptr;
}

void World::RegisterComponent(Component* component)
{
    _registeredComponents[typeid(*component)].push_back(component);
}

void World::UnRegisterComponent(Component* component)
{
    auto componentGroup = _registeredComponents.find(typeid(*component));
    if (componentGroup == _registeredComponents.end())
    {
        dvl::Log(dvl::LogLevel::Error, "Couldn't find type of component to be erased!");
        return;
    }

    std::vector<Component*>& components = componentGroup->second;

    auto it = std::find(components.begin(), components.end(), component);
    
    if (it == components.end())
    {
        dvl::Log(dvl::LogLevel::Error, "Couldn't find component to be erased!");
        return ;
    }

    components.erase(it);

    if (components.empty())
    {
        _registeredComponents.erase(componentGroup);
    }
}
    
const std::vector<std::unique_ptr<Entity>>& World::GetEntities() const
{
    return _entities;
}