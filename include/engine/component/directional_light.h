#pragma once

#include <dvl/math/math.h>

#include "engine/component/light.h"

class DirectionalLight : public Light
{
public:
    DirectionalLight(Entity& entity);
    
    COMPONENT_TYPES(DirectionalLight, Light, Component)
    COMPONENT_FIELDS(Light, direction)

    dvl::Vec3 direction = dvl::Vec3(0.0f, -1.0f, 0.0f);
};
