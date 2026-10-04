#pragma once

#include <dvl/math/math.h>

#include "engine/component/component.h"

class Light : public Component
{
public:
    Light(Entity& entity);

    COMPONENT_TYPES(Light, Component)
    COMPONENT_FIELDS(Component, color, intensity)

    dvl::Vec3 color = dvl::Vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
};