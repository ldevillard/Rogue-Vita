#include "engine/core/entity.h"

const std::vector<std::unique_ptr<Component>>& Entity::GetComponents() const
{
    return _components;
}