#pragma once

#include <vector>

#include "engine/core/entity.h"

class World
{
public:
    Entity* CreateEntity();
    void DestroyEntity(unsigned int id);
    
    Entity* FindEntity(unsigned int id);
    const Entity* FindEntity(unsigned int id) const;
    
    std::vector<Entity*> GetEntities();
    const std::vector<const Entity*> GetEntities() const;

private:
    unsigned int _nextId = 1;
    std::vector<std::unique_ptr<Entity>> _entities;
};
