#pragma once

#include "engine/component/component.h"
#include "engine/core/transform.h"
#include "engine/render/material.h"
#include "engine/render/mesh.h"

class MeshRenderer : public Component
{
public:
    MeshRenderer(Entity& entity, MeshHandle meshHandle, const Material& material);
    COMPONENT_TYPES(MeshRenderer, Component)

    Transform localTransform;

    MeshHandle meshHandle;
    const Material material;
};
