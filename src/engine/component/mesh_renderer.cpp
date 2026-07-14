#include "engine/component/mesh_renderer.h"

MeshRenderer::MeshRenderer(const Mesh* mesh, const Material& material)
    : mesh(mesh), material(material)
{
}

bool MeshRenderer::IsValid() const
{
    return mesh != nullptr && mesh->IsValid() &&
           material.renderPipeline != nullptr && material.renderPipeline->IsValid();
}
