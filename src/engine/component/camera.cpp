#include "engine/component/camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <dvl/log/log.h>

#include "engine/core/entity.h"

Camera::Camera(Entity& entity, float screenWidth, float screenHeight, ProjectionType projectionType)
    : Component(entity)
{
    constexpr float NearPlane = 0.1f;
    constexpr float FarPlane = 100.0f;
    
    const float aspectRatio = screenWidth / screenHeight;
    
    switch (projectionType)
    {
    case Perspective:
        _projection = glm::perspective(glm::radians(60.0f), aspectRatio, NearPlane, FarPlane);
        break;
        
    case Orthographic:
    {
        constexpr float OrthographicSize = 6.0f;
        const float halfHeight = OrthographicSize * 0.5f;
        const float halfWidth = halfHeight * aspectRatio;

        _projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, NearPlane, FarPlane);
        break;
    }

    default:
        _projection = glm::perspective(glm::radians(60.0f), aspectRatio, NearPlane, FarPlane);
        break;
    }

    _view = glm::mat4{1.0f};
}

void Camera::UpdateViewMatrix()
{
    _view = glm::inverse(entity.transform.GetMatrix());
}

const glm::mat4& Camera::GetViewMatrix() const
{
    return _view;
}

const glm::mat4& Camera::GetProjectionMatrix() const
{
    return _projection;
}
