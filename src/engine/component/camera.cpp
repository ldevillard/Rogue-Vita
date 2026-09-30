#include "engine/component/camera.h"

#include <dvl/log/log.h>

#include "engine/core/entity.h"

Camera::Camera(Entity& entity)
    : Component(entity)
{
}

Camera::Camera(Entity& entity, float screenWidth, float screenHeight, ProjectionType projectionType)
    : Component(entity), _screenWidth(screenWidth), _screenHeight(screenHeight), _projectionType(projectionType)
{
}

void Camera::Start()
{
    constexpr float NearPlane = 0.1f;
    constexpr float FarPlane = 100.0f;
    
    const float aspectRatio = _screenWidth / _screenHeight;
    
    switch (_projectionType)
    {
    case Perspective:
        _projection = dvl::Mat4::Perspective(dvl::Radians(60.0f), aspectRatio, NearPlane, FarPlane);
        break;
        
    case Orthographic:
    {
        constexpr float OrthographicSize = 4.0f;
        const float halfHeight = OrthographicSize * 0.5f;
        const float halfWidth = halfHeight * aspectRatio;

        _projection = dvl::Mat4::Orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, NearPlane, FarPlane);
        break;
    }

    default:
        _projection = dvl::Mat4::Perspective(dvl::Radians(60.0f), aspectRatio, NearPlane, FarPlane);
        break;
    }

    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    _view = dvl::Mat4::Inverse(entity.transform.GetMatrix());
}

const dvl::Mat4& Camera::GetViewMatrix() const
{
    return _view;
}

const dvl::Mat4& Camera::GetProjectionMatrix() const
{
    return _projection;
}
