#pragma once

#include <dvl/math/math.h>

#include "engine/component/component.h"

// It is assumed that a collider should live in an entity that has no parent, so the collider lives in world space
class Collider : public Component
{
public:
    Collider(Entity& entity);
    
    COMPONENT_TYPES(Collider, Component)
    COMPONENT_FIELDS(Component, )
};