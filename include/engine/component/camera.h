#pragma once

#include <glm/glm.hpp>

#include "engine/component/component.h"

class Camera : public Component
{
public:
    Camera(const float screenWidth, const float screenHeight);

    bool IsValid() const override;

    // TODO: should return a reference, the view matrix should be updated once per frame
    glm::mat4 GetViewMatrix() const;
    const glm::mat4& GetProjectionMatrix() const;

private:
    glm::mat4 _projection;
};
