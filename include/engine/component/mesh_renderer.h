#pragma once

#include "engine/component/component.h"
#include "engine/render/material.h"
#include "engine/render/mesh.h"

class MeshRenderer : public Component
{
public:
    MeshRenderer(const Mesh* mesh, const Material& material);

    bool IsValid() const override;

    const Mesh* mesh = nullptr;
    const Material material;
};
