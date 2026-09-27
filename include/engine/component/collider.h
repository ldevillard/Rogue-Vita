#pragma once

#include <dvl/math/math.h>

#include "engine/component/component.h"

class Collider : public Component
{
public:
    Collider(Entity& entity);
    COMPONENT_TYPES(Collider, Component)
};