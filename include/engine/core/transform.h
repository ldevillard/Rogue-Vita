#pragma once

#include <glm/glm.hpp>

// TODO: Compute matrices and vectors once per frame, maybe a dirty flag system may work
class Transform
{
public:
    glm::vec3 position {0.0f};
    // TODO: Support quat to avoid gimball lock
    glm::vec3 rotation {0.0f};
    glm::vec3 scale {1.0f};

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

    // TODO: Update with quat when it will be available
    void LookAt(const glm::vec3& target);
    glm::mat4 GetMatrix() const;
};
