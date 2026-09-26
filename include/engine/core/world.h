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
    void DestroyEntity(unsigned int id);
    
    Entity* FindEntity(unsigned int id);
    const Entity* FindEntity(unsigned int id) const;

    void RegisterComponent(Component* component);
    void UnRegisterComponent(Component* component);
    
    const std::vector<std::unique_ptr<Entity>>& GetEntities() const;

private:
    unsigned int _nextId = 1;

    std::vector<std::unique_ptr<Entity>> _entities;

    // This is a filtered view of all components, there is no ownership on them
    std::unordered_map<std::type_index, std::vector<Component*>> _registeredComponents;
};
