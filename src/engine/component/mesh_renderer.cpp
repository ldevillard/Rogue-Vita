#include "engine/component/mesh_renderer.h"

MeshRenderer::MeshRenderer(Entity& entity, MeshHandle meshHandle, const Material& material)
    : Component(entity), meshHandle(meshHandle), material(material)
{
}
