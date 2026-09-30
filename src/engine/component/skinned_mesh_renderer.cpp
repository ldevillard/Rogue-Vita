#include "engine/component/skinned_mesh_renderer.h"

SkinnedMeshRenderer::SkinnedMeshRenderer(Entity& entity)
    : MeshRenderer(entity)
{
}

SkinnedMeshRenderer::SkinnedMeshRenderer(Entity& entity, MeshHandle meshHandle, const Material& material, SkeletonHandle skeletonHandle)
    : MeshRenderer(entity, meshHandle, material), skeletonHandle(skeletonHandle)
{
}