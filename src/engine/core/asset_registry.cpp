#include "engine/core/asset_registry.h"

#include "engine/render/vertex.h"
#include "engine/render/render_pipeline.h"

AssetRegistry::AssetRegistry(Renderer& renderer) 
    : _renderer(renderer)
{
    loadCubePrimitive();
    loadMaterials();
}

AssetRegistry::~AssetRegistry()
{
    for (std::pair<const MeshHandle, Mesh>& meshPair : _meshes)
    {
        _renderer.DestroyMesh(meshPair.second);
    }

    for (std::pair<const RenderPipelineHandle, RenderPipeline>& pipelinePair : _pipelines)
    {
        _renderer.DestroyRenderPipeline(pipelinePair.second);
    }
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

const Mesh& AssetRegistry::GetCubeMesh() const
{
    return _meshes.at(_cubeMeshHandle);
}

const Material AssetRegistry::GetSolidMaterialInstance() const
{
    return _materials.at(_solidMaterialHandle);
}

const Material AssetRegistry::GetWireframeMaterialInstance() const
{
    return _materials.at(_wireframeMaterialHandle);
}

void AssetRegistry::loadCubePrimitive()
{
    const VertexPositionNormalColor CubeVertices[] =
    {
        // Front
        {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        { 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        {-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        { 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, {1.0f, 1.0f, 1.0f, 1.0f}},

        // Back
        { 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        { 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        {-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, {1.0f, 1.0f, 1.0f, 1.0f}},

        // Left
        {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        {-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        {-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},

        // Right
        { 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        { 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        { 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        { 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},

        // Top
        {-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        { 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        { 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},

        // Bottom
        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        { 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        {-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}},
        { 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, {1.0f, 1.0f, 1.0f, 1.0f}}
    };

    const std::uint16_t CubeIndices[] =
    {
        // Front
        0, 1, 3,
        0, 3, 2,
    
        // Back
        4, 5, 7,
        4, 7, 6,
    
        // Left
        8, 9, 11,
        8, 11, 10,
    
        // Right
        12, 13, 15,
        12, 15, 14,
    
        // Top
        16, 17, 19,
        16, 19, 18,
    
        // Bottom
        20, 21, 23,
        20, 23, 22
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
        {"aPosition", dvl::VertexFormat::Float3, offsetof(VertexPositionNormalColor, x)},
        {"aNormal", dvl::VertexFormat::Float3, offsetof(VertexPositionNormalColor, nx)},
        {"aColor", dvl::VertexFormat::Float4, offsetof(VertexPositionNormalColor, color)}
    };

    Material solidMaterial = {};
    Material wireframeMaterial = {};

    RenderPipelineDesc pipelineDesc = {};
    pipelineDesc.vertexShaderPath = "app0:/assets/shaders/vertex.vert";
    pipelineDesc.fragmentShaderPath = "app0:/assets/shaders/fragment.frag";
    pipelineDesc.attributes = attributes;
    pipelineDesc.attributeCount = sizeof(attributes) / sizeof(attributes[0]);
    pipelineDesc.vertexStride = sizeof(VertexPositionNormalColor);
    pipelineDesc.depthStencilState.depthTestEnabled = true;
    pipelineDesc.depthStencilState.depthWriteEnabled = true;

    RenderPipeline solidRenderPipeline = _renderer.CreateRenderPipeline(pipelineDesc);
    pipelineDesc.rasterizerState.fillMode = dvl::FillMode::Wireframe;
    RenderPipeline wireframeRenderPipeline= _renderer.CreateRenderPipeline(pipelineDesc);
    
    RenderPipelineHandle solidPipelineHandle = {};
    solidPipelineHandle.id = _nextPipelineId++;
    
    RenderPipelineHandle wireframePipelineHandle = {};
    wireframePipelineHandle.id = _nextPipelineId++;
    
    _pipelines.emplace(solidPipelineHandle, solidRenderPipeline);
    _pipelines.emplace(wireframePipelineHandle, wireframeRenderPipeline);
    
    solidMaterial.renderPipeline = &_pipelines.at(solidPipelineHandle);
    wireframeMaterial.renderPipeline = &_pipelines.at(wireframePipelineHandle);

    _solidMaterialHandle.id = _nextMaterialId++;
    _wireframeMaterialHandle.id = _nextMaterialId++;

    _materials.emplace(_solidMaterialHandle, solidMaterial);
    _materials.emplace(_wireframeMaterialHandle, wireframeMaterial);
}
