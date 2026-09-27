#pragma once

#include "engine/component/component.h"

class Behavior : public Component
{
public:
    Behavior(Entity& entity);
    COMPONENT_TYPES(Behavior, Component)

    virtual void Update(float deltaTime) = 0;
};