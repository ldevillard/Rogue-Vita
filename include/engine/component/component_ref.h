#pragma once

#include "engine/core/world.h"

template<typename T>
struct ComponentRef
{
    unsigned int id = 0;

    T* Get(World& world) const
    {
        return dynamic_cast<T*>(world.FindComponent(id));
    }
};