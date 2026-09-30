#include "engine/core/world.h"

#include <algorithm>
#include <dvl/log/log.h>
#include <string>
#include <typeinfo>

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
     for (auto it = _entities.begin(); it != _entities.end(); it++)
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

Entity* World::Instantiate(const Entity& source)
{
    assert(&source.GetWorld() == this && "Instantiate requires the source to belong to this world");
    
    Entity* clonedEntity = CreateEntity();
    clonedEntity->transform = source.transform;

    ReferenceMapping refMap;
    refMap.entityIds[source.id] = clonedEntity->id;

    const std::vector<std::unique_ptr<Component>>& sourceComponents = source.GetComponents();
    std::vector<Component*> clonedComponents(sourceComponents.size(), nullptr);

    // First step, creation and attachment of cloned components
    for (size_t i = 0; i < sourceComponents.size(); i++)
    {
        const Component& original = *sourceComponents[i];

        refMap.componentIds[original.id] = 0;

        std::unique_ptr<Component> clone = original.CreateEmpty(*clonedEntity);

        if (clone == nullptr)
        {
            const std::string message = "Instantiate: failed to clone component of type " + std::string(typeid(original).name());
            dvl::Log(dvl::LogLevel::Error, message.c_str());
            continue;
        }

        if (typeid(*clone) != typeid(original))
        {
            dvl::Log(dvl::LogLevel::Error, "Instantiate: CreateEmpty returned a different component type");
            continue;
        }

        Component* attached = clonedEntity->AttachComponent(std::move(clone));
        if (attached == nullptr)
        {
            const std::string message = "Instantiate: failed to attach component of type " + std::string(typeid(original).name());
            dvl::Log(dvl::LogLevel::Error, message.c_str());
            continue;
        }

        clonedComponents[i] = attached;
        refMap.componentIds[original.id] = attached->id;
    }

    // Second steps, resolve references, copy fields..
    for (std::size_t i = 0; i < sourceComponents.size(); i++)
    {
        Component* cloned = clonedComponents[i];

        if (cloned != nullptr)
            cloned->CopyFrom(*sourceComponents[i], refMap);
    }

    return clonedEntity;
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