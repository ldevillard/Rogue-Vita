#include "engine/component/camera.h"

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
        _projection = dvl::Mat4::Perspective(dvl::Radians(60.0f), aspectRatio, NearPlane, FarPlane);
        break;
        
    case Orthographic:
    {
        constexpr float OrthographicSize = 7.0f;
        const float halfHeight = OrthographicSize * 0.5f;
        const float halfWidth = halfHeight * aspectRatio;

        _projection = dvl::Mat4::Orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, NearPlane, FarPlane);
        break;
    }

    default:
        _projection = dvl::Mat4::Perspective(dvl::Radians(60.0f), aspectRatio, NearPlane, FarPlane);
        break;
    }

    _view = dvl::Mat4::Identity();
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
