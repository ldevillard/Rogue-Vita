#include "engine/core/world.h"

#include <algorithm>
#include <dvl/log/log.h>
#include <string>
#include <typeinfo>

#include "engine/component/component_ref.h"
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

void World::DestroyEntity(EntityRef entityRef)
{
    if (entityRef.Get(*this) == nullptr)
        return;

    for (const EntityRef& pendingEntity : _pendingDestroyEntities)
    {
        if (pendingEntity.id == entityRef.id)
            return;
    }

    _pendingDestroyEntities.push_back(entityRef);
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

    _componentsById[component->id] = component;
    _pendingStartComponents.push_back(component->ToRef<Component>());
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

    _componentsById.erase(component->id);
}

Entity* World::Instantiate(const Entity& source)
{
    assert(&source.GetWorld() == this && "Instantiate requires the source to belong to this world");

    ReferenceMapping refMap;
    std::vector<std::pair<const Component*, Component*>> clonedComponents;
    Entity* clonedRoot = instantiateRecursive(source, nullptr, refMap, clonedComponents);

    for (const std::pair<const Component*, Component*>& clonedComponent : clonedComponents)
    {
        clonedComponent.second->CopyFrom(*clonedComponent.first, refMap);
    }

    return clonedRoot;
}

void World::StartPendingComponents()
{
    for (const ComponentRef<Component>& componentRef : _pendingStartComponents)
    {
        Component* component = componentRef.Get(*this);
        if (component != nullptr)
            component->Start();
    }

    _pendingStartComponents.clear();
}
    
void World::FlushDestroyedEntities()
{
    for (const EntityRef& entityRef : _pendingDestroyEntities)
    {
        destroyEntity(entityRef);
    }

    _pendingDestroyEntities.clear();
}

const std::vector<std::unique_ptr<Entity>>& World::GetEntities() const
{
    return _entities;
}

void World::destroyEntity(EntityRef entityRef)
{
    Entity* entity = entityRef.Get(*this);
    if (entity == nullptr)
        return;

    entity->SetParent(nullptr);

    const std::vector<EntityRef> children = entity->GetChildren();
    for (const EntityRef& child : children)
    {
        destroyEntity(child);
    }

    for (std::size_t i = 0; i < _entities.size(); i++)
    {
        if (_entities[i]->id == entityRef.id)
        {
            _entities.erase(_entities.begin() + i);
            return;
        }
    }
}

Entity* World::instantiateRecursive(const Entity& source, Entity* parent, ReferenceMapping& refMap,
                                    std::vector<std::pair<const Component*, Component*>>& clonedComponents)
{
    Entity* clonedEntity = CreateEntity();
    clonedEntity->transform = source.transform;
    clonedEntity->SetParent(parent);

    refMap.entityIds[source.id] = clonedEntity->id;

    for (const std::unique_ptr<Component>& sourceComponent : source.GetComponents())
    {
        const Component& original = *sourceComponent;
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

        refMap.componentIds[original.id] = attached->id;
        clonedComponents.emplace_back(&original, attached);
    }

    for (const EntityRef& childRef : source.GetChildren())
    {
        Entity* child = childRef.Get(*this);
        if (child != nullptr)
            instantiateRecursive(*child, clonedEntity, refMap, clonedComponents);
    }

    return clonedEntity;
}
