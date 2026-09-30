#pragma once

#include <dvl/math/mat.h>

#include "engine/component/component.h"

class Camera : public Component
{
public:
    enum ProjectionType
    {
        Perspective,
        Orthographic
    };

    Camera(Entity& entity);
    Camera(Entity& entity, float screenWidth, float screenHeight, ProjectionType projectionType = Perspective);
    
    COMPONENT_TYPES(Camera, Component)
    COMPONENT_FIELDS(Component, _screenWidth, _screenHeight, _projectionType)

    void Start() override;
    void UpdateViewMatrix();

    const dvl::Mat4& GetViewMatrix() const;
    const dvl::Mat4& GetProjectionMatrix() const;

private:
    float _screenWidth = 1.0f;
    float _screenHeight = 1.0f;
    ProjectionType _projectionType = Perspective;

    dvl::Mat4 _view = dvl::Mat4::Identity();
    dvl::Mat4 _projection = dvl::Mat4::Identity();
};
