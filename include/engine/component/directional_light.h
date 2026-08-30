#pragma once

#include <dvl/math/math.h>

#include "engine/component/component.h"

class DirectionalLight : public Component
{
public:
    DirectionalLight(Entity& entity);

    dvl::Vec3 direction = dvl::Vec3(0.0f, -1.0f, 0.0f);
    dvl::Vec3 color = dvl::Vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
};
