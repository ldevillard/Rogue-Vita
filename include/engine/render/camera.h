#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera(const float screenWidth, const float screenHeight);

    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetProjectionMatrix() const;

private:
    glm::mat4 _view;
    glm::mat4 _projection;
};