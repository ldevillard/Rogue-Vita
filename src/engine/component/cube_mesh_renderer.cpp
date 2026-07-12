#include "engine/component/cube_mesh_renderer.h"

#include "engine/render/renderer.h"
#include "engine/render/vertex.h"

const VertexPositionColor CubeVertices[] =
{
    // Back
    {-0.5f, -0.5f, -0.5f, {0.0f, 0.0f, 0.0f, 1.0f}},
    { 0.5f, -0.5f, -0.5f, {1.0f, 0.0f, 0.0f, 1.0f}},
    {-0.5f,  0.5f, -0.5f, {0.0f, 1.0f, 0.0f, 1.0f}},
    { 0.5f,  0.5f, -0.5f, {1.0f, 1.0f, 0.0f, 1.0f}},

    // Front
    {-0.5f, -0.5f,  0.5f, {0.0f, 0.0f, 1.0f, 1.0f}},
    { 0.5f, -0.5f,  0.5f, {1.0f, 0.0f, 1.0f, 1.0f}},
    {-0.5f,  0.5f,  0.5f, {0.0f, 1.0f, 1.0f, 1.0f}},
    { 0.5f,  0.5f,  0.5f, {1.0f, 1.0f, 1.0f, 1.0f}}
};

const std::uint16_t CubeIndices[] =
{
    // Front
    4, 5, 7,
    4, 7, 6,

    // Back
    1, 0, 2,
    1, 2, 3,

    // Left
    0, 4, 6,
    0, 6, 2,

    // Right
    5, 1, 3,
    5, 3, 7,

    // Top
    6, 7, 3,
    6, 3, 2,

    // Bottom
    0, 1, 5,
    0, 5, 4
};

CubeMeshRenderer::CubeMeshRenderer(Renderer& renderer, const Material* material)
    : MeshRenderer(nullptr, material), _renderer(renderer)
{
    MeshDesc desc;
    desc.vertexData = CubeVertices;
    desc.vertexDataSize = sizeof(CubeVertices);
    desc.indices = CubeIndices;
    desc.indexCount = sizeof(CubeIndices) / sizeof(CubeIndices[0]);

    if (_renderer.CreateMesh(desc, _mesh))
    {
        mesh = &_mesh;
    }
}

CubeMeshRenderer::~CubeMeshRenderer()
{
    _renderer.DestroyMesh(_mesh);
    mesh = nullptr;
}
