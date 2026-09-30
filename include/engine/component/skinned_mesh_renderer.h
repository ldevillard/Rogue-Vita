#pragma once

#include "engine/component/mesh_renderer.h"
#include "engine/render/skeleton.h"

class SkinnedMeshRenderer : public MeshRenderer
{
public:
    SkinnedMeshRenderer(Entity& entity);
    SkinnedMeshRenderer(Entity& entity, MeshHandle meshHandle, const Material& material, SkeletonHandle skeletonHandle);
    
    COMPONENT_TYPES(SkinnedMeshRenderer, MeshRenderer, Component)
    COMPONENT_FIELDS(MeshRenderer, skeletonHandle)

    SkeletonHandle skeletonHandle;
};