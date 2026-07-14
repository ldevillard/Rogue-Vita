#include "engine/component/directional_light.h"

#include <glm/geometric.hpp>

bool DirectionalLight::IsValid() const
{
    return glm::dot(direction, direction) > 0.0f && intensity >= 0.0f;
}
