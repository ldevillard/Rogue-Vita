#include "engine/render/camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const float screenWidth, const float screenHeight)
{
    _view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	_projection = glm::perspective(glm::radians(60.0f), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

}

const glm::mat4& Camera::GetViewMatrix() const
{
    return _view;
}

const glm::mat4& Camera::GetProjectionMatrix() const
{
    return _projection;
}