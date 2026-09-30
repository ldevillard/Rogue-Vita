#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

class Component;
class Entity;

class World
{
public:
    World();
    ~World();

    Entity* CreateEntity();
    // TODO: Add a destroy flush system to avoid destroying entities during update loops
    void DestroyEntity(unsigned int id);
    
    Entity* FindEntity(unsigned int id);
    Component* FindComponent(unsigned int id);

    void RegisterComponent(Component* component);
    void UnRegisterComponent(Component* component);

    Entity* Instantiate(const Entity& source);

    void StartPendingComponents();
    
    const std::vector<std::unique_ptr<Entity>>& GetEntities() const;

    // TODO: Check if use a ForEachComponent can be relevent
    template <typename T>
    const std::vector<T*> GetComponents()
    {
        const std::vector<Component*>& components = _registeredComponents[typeid(T)];

        std::vector<T*> result;
        result.reserve(components.size());

        for (Component* component : components)
        {
            result.push_back(static_cast<T*>(component));
        }

        return result;
    }

private:
    unsigned int _nextEntityId = 1;
    unsigned int _nextComponentId = 1;

    std::vector<std::unique_ptr<Entity>> _entities;
    std::unordered_map<unsigned int, Component*> _componentsById;

    std::vector<Component*> _pendingStartComponents;

    // This is a filtered view of all components, there is no ownership on them
    std::unordered_map<std::type_index, std::vector<Component*>> _registeredComponents;
};
