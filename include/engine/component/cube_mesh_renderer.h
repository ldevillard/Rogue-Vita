#pragma once

#include "engine/component/mesh_renderer.h"

class Renderer;

class CubeMeshRenderer : public MeshRenderer
{
public:
    CubeMeshRenderer(Renderer& renderer, const Material* material);
    ~CubeMeshRenderer() override;

private:
    Renderer& _renderer;
    Mesh _mesh;
};
