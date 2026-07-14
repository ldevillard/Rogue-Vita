#include "engine/component/camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "engine/core/entity.h"

Camera::Camera(const float screenWidth, const float screenHeight)
{
	_projection = glm::perspective(glm::radians(60.0f), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);
}

bool Camera::IsValid() const
{
    return entity != nullptr;
}

glm::mat4 Camera::GetViewMatrix() const
{
    if (entity == nullptr)
        return glm::mat4{1.0f};

    // TODO: Cache the view matrix and update it once per frame
    return glm::inverse(entity->transform.GetMatrix());
}

const glm::mat4& Camera::GetProjectionMatrix() const
{
    return _projection;
}
