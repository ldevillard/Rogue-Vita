#include "engine/core/world.h"

Entity* World::CreateEntity()
{
    std::unique_ptr<Entity> entity = std::make_unique<Entity>();
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
    
const std::vector<std::unique_ptr<Entity>>& World::GetEntities() const
{
    return _entities;
}