#pragma once

#include "engine/component/mesh_renderer.h"
#include "engine/render/skeleton.h"

class SkinnedMeshRenderer : public MeshRenderer
{
public:
    SkinnedMeshRenderer(Entity& entity, const Mesh* mesh, const Material& material, const Skeleton* skeleton);
    COMPONENT_TYPES(SkinnedMeshRenderer, MeshRenderer, Component)

    // TODO: Use handle instead
    const Skeleton* skeleton = nullptr;
};