#pragma once

#include <glm/glm.hpp>

#include "engine/component/component.h"

class Camera : public Component
{
public:
    enum ProjectionType
    {
        Perspective,
        Orthographic
    };

    Camera(Entity& entity, float screenWidth, float screenHeight, ProjectionType projectionType = Perspective);

    void UpdateViewMatrix();

    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetProjectionMatrix() const;

private:
    glm::mat4 _view;
    glm::mat4 _projection;
};
