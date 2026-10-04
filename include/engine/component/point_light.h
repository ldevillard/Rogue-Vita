#pragma once

#include "engine/component/light.h"

class PointLight : public Light
{
public:
    PointLight(Entity& entity);

    COMPONENT_TYPES(PointLight, Light, Component)
    COMPONENT_FIELDS(Light, radius)

    float radius = 1.0f;
    float falloff = 1.0f;
};