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

    Camera(Entity& entity, float screenWidth, float screenHeight, ProjectionType projectionType = Perspective);

    void UpdateViewMatrix();

    const dvl::Mat4& GetViewMatrix() const;
    const dvl::Mat4& GetProjectionMatrix() const;

private:
    dvl::Mat4 _view;
    dvl::Mat4 _projection;
};
