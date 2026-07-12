#include "engine/component/mesh_renderer.h"

MeshRenderer::MeshRenderer(const Mesh* mesh, const Material* material)
{
    this->mesh = mesh;
    this->material = material;
}