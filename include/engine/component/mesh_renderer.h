#pragma once

#include "engine/component/component.h"
#include "engine/core/transform.h"
#include "engine/render/material.h"
#include "engine/render/mesh.h"

class MeshRenderer : public Component
{
public:
    MeshRenderer(Entity& entity, const Mesh* mesh, const Material& material);

    Transform localTransform;

    // TODO: Use handle instead
    const Mesh* mesh = nullptr;
    const Material material;
};
