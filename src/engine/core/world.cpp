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
    entity->id = _nextEntityId++;

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

Component* World::FindComponent(unsigned int id)
{
    auto it = _componentsById.find(id);
    return it != _componentsById.end() ? it->second : nullptr;
}

void World::RegisterComponent(Component* component)
{
    component->id = _nextComponentId++;

    for (std::type_index type : component->GetTypes())
    {
        _registeredComponents[type].push_back(component);
    }

    _pendingStartComponents.push_back(component);
    _componentsById[component->id] = component;
}

void World::UnRegisterComponent(Component* component)
{
    for (std::type_index type : component->GetTypes())
    {
        auto componentGroup = _registeredComponents.find(type);
        if (componentGroup == _registeredComponents.end())
        {
            dvl::Log(dvl::LogLevel::Error, "Couldn't find type of component to be erased!");
            continue;
        }
        
        std::vector<Component*>& components = componentGroup->second;
        
        auto it = std::find(components.begin(), components.end(), component);
        
        if (it == components.end())
        {
            dvl::Log(dvl::LogLevel::Error, "Couldn't find component to be erased!");
            continue;
        }
        
        components.erase(it);
        
        if (components.empty())
        {
            _registeredComponents.erase(componentGroup);
        }
    }

    auto it = std::find(_pendingStartComponents.begin(), _pendingStartComponents.end(), component);
    if (it != _pendingStartComponents.end())
    {
        _pendingStartComponents.erase(it);
    }

    _componentsById.erase(component->id);
}

void World::StartPendingComponents()
{
    for (Component* component : _pendingStartComponents)
    {
        component->Start();
    }

    _pendingStartComponents.clear();
}
    
const std::vector<std::unique_ptr<Entity>>& World::GetEntities() const
{
    return _entities;
}