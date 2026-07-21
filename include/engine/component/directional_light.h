#pragma once

#include <glm/glm.hpp>

#include "engine/component/component.h"

class DirectionalLight : public Component
{
public:
    DirectionalLight(Entity& entity);

    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
};
