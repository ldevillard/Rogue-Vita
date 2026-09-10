#include "engine/component/skinned_mesh_renderer.h"

SkinnedMeshRenderer::SkinnedMeshRenderer(Entity& entity, const Mesh* mesh, const Material& material, const Skeleton* skeleton)
    : MeshRenderer(entity, mesh, material), skeleton(skeleton)
{
}