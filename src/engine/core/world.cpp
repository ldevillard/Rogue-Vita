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
    
std::vector<Entity*> World::GetEntities()
{
    std::vector<Entity*> entities;
    entities.reserve(_entities.size());

    for (const std::unique_ptr<Entity>& entity : _entities)
    {
        entities.push_back(entity.get());
    }

    return entities;
}

const std::vector<const Entity*> World::GetEntities() const
{
    std::vector<const Entity*> entities;
    entities.reserve(_entities.size());

    for (const std::unique_ptr<Entity>& entity : _entities)
    {
        entities.push_back(entity.get());
    }

    return entities;
}
