#pragma once

#include "engine/component/mesh_renderer.h"
#include "engine/render/skeleton.h"

class SkinnedMeshRenderer : public MeshRenderer
{
public:
    SkinnedMeshRenderer(Entity& entity, const Mesh* mesh, const Material& material, const Skeleton* skeleton);

    // TODO: Use handle instead
    const Skeleton* skeleton = nullptr;
};