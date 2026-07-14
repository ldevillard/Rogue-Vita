#include "engine/core/asset_registry.h"

#include "engine/render/vertex.h"
#include "engine/render/render_pipeline.h"

AssetRegistry::AssetRegistry(Renderer& renderer) 
    : _renderer(renderer)
{
    loadCubePrimitive();
    loadMaterials();
}

const Mesh* AssetRegistry::GetMesh(const MeshHandle& meshHandle) const
{
    const auto it = _meshes.find(meshHandle);

    if (it != _meshes.end())
    {
        return &it->second;
    }

    return nullptr;
}

const Material* AssetRegistry::GetMaterial(const MaterialHandle& materialHandle) const
{
    const auto it = _materials.find(materialHandle);

    if (it != _materials.end())
    {
        return &it->second;
    }

    return nullptr;
}

const Mesh* AssetRegistry::GetCubeMesh() const
{
    return &_meshes.at(_cubeMeshHandle);
}

const Material* AssetRegistry::GetSolidMaterial() const
{
    return &_materials.at(_solidMaterialHandle);
}

const Material* AssetRegistry::GetWireframeMaterial() const
{
    return &_materials.at(_wireframeMaterialHandle);
}

void AssetRegistry::loadCubePrimitive()
{
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

    MeshDesc desc = {};
    desc.vertexData = CubeVertices;
    desc.vertexDataSize = sizeof(CubeVertices);
    desc.indices = CubeIndices;
    desc.indexCount = sizeof(CubeIndices) / sizeof(CubeIndices[0]);

    Mesh cubeMesh = {};

    if (_renderer.CreateMesh(desc, cubeMesh))
    {
        _cubeMeshHandle.id = _nextMeshId++;
        _meshes.emplace(_cubeMeshHandle, cubeMesh);
    }
}

void AssetRegistry::loadMaterials()
{
    const dvl::VertexAttribute attributes[] =
    {
        {"aPosition", dvl::VertexFormat::Float3, offsetof(VertexPositionColor, x)},
        {"aColor", dvl::VertexFormat::Float4, offsetof(VertexPositionColor, color)}
    };

    Material solidMaterial = {};
    Material wireframeMaterial = {};

    RenderPipelineDesc pipelineDesc = {};
    pipelineDesc.vertexShaderPath = "app0:/assets/shaders/vertex.vert";
    pipelineDesc.fragmentShaderPath = "app0:/assets/shaders/fragment.frag";
    pipelineDesc.attributes = attributes;
    pipelineDesc.attributeCount = sizeof(attributes) / sizeof(attributes[0]);
    pipelineDesc.vertexStride = sizeof(VertexPositionColor);
    pipelineDesc.depthStencilState.depthTestEnabled = true;
    pipelineDesc.depthStencilState.depthWriteEnabled = true;

    solidMaterial.pipeline = _renderer.CreateRenderPipeline(pipelineDesc);
    pipelineDesc.rasterizerState.fillMode = dvl::FillMode::Wireframe;
    wireframeMaterial.pipeline = _renderer.CreateRenderPipeline(pipelineDesc);

    _solidMaterialHandle.id = _nextMaterialId++;
    _wireframeMaterialHandle.id = _nextMaterialId;

    _materials.emplace(_solidMaterialHandle, solidMaterial);
    _materials.emplace(_wireframeMaterialHandle, wireframeMaterial);
}
