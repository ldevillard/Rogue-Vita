#include "engine/component/mesh_renderer.h"

MeshRenderer::MeshRenderer(const Mesh* mesh, const Material* material)
    : mesh(mesh), material(material)
{
}

bool MeshRenderer::IsValid() const
{
    return mesh != nullptr && material != nullptr &&
           mesh->IsValid() && material->pipeline.IsValid();
}
