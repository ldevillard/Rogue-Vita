#include "engine/core/entity_ref.h"

#include "engine/core/world.h"

Entity* EntityRef::Get(World& world) const
{
    return world.FindEntity(id);
}