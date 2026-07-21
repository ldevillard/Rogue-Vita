#include "engine/component/mesh_renderer.h"

MeshRenderer::MeshRenderer(Entity& entity, const Mesh* mesh, const Material& material)
    : Component(entity), mesh(mesh), material(material)
{
}
