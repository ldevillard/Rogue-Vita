#include "engine/core/transform.h"

#include <glm/gtc/matrix_transform.hpp>

void Transform::LookAt(const glm::vec3& target)
{
    const glm::vec3 delta = target - position;

    if (glm::dot(delta, delta) < 0.000001f)
        return;

    const glm::mat4 worldMatrix = glm::inverse(glm::lookAt(position,target, glm::vec3{0.0f, 1.0f, 0.0f}));

    rotation.y = std::asin(glm::clamp(worldMatrix[2][0], -1.0f, 1.0f));
    rotation.x = std::atan2(-worldMatrix[2][1], worldMatrix[2][2]);
    rotation.z = std::atan2(-worldMatrix[1][0], worldMatrix[0][0]);
}

glm::mat4 Transform::GetMatrix() const
{
    glm::mat4 matrix {1.0f};

    matrix = glm::translate(matrix, position);

    matrix = glm::rotate(matrix, rotation.x, glm::vec3{1.0f, 0.0f, 0.0f});
    matrix = glm::rotate(matrix, rotation.y, glm::vec3{0.0f, 1.0f, 0.0f});
    matrix = glm::rotate(matrix, rotation.z, glm::vec3{0.0f, 0.0f, 1.0f});

    matrix = glm::scale(matrix, scale);

    return matrix;
}
