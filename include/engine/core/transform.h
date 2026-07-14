#pragma once

#include <glm/glm.hpp>

// TODO: Make a class instead of a struct
struct Transform
{
    glm::vec3 position {0.0f};
    // TODO: Support quat to avoid gimball lock
    glm::vec3 rotation {0.0f};
    glm::vec3 scale {1.0f};

    // TODO: Update with quat when it will be available
    void LookAt(const glm::vec3& target);
    glm::mat4 GetMatrix() const;
};
