#pragma once

#include <glm/glm.hpp>

#include "engine/component/component.h"

class DirectionalLight : public Component
{
public:
    bool IsValid() const override;

    glm::vec3 direction = {0.0f, -1.0f, 0.0f};
    glm::vec3 color = {1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;
};