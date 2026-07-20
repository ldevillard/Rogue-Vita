#include "engine/core/asset_registry.h"

#include <cstring>
#include <fstream>

#include <dvl/asset/mesh_format.h>
#include <dvl/asset/texture_format.h>

#include "engine/render/vertex.h"
#include "engine/render/renderer.h"
#include "engine/render/render_pipeline.h"

void AssetRegistry::Initialize(Renderer& renderer)
{
    loadCubePrimitive(renderer);
    loadDefaultTexture(renderer);
    loadMaterials(renderer);
}

void AssetRegistry::Shutdown(Renderer& renderer)
{
    for (dvl::TextureHandle texture : _textures)
    {
        renderer.DestroyTexture(texture);
    }

    for (std::pair<const MeshHandle, Mesh>& meshPair : _meshes)
    {
        renderer.DestroyMesh(meshPair.second);
    }

    for (std::pair<const RenderPipelineHandle, RenderPipeline>& pipelinePair : _pipelines)
    {
        renderer.DestroyRenderPipeline(pipelinePair.second);
    }
}

MeshHandle AssetRegistry::LoadMesh(const std::filesystem::path& path, Renderer& renderer)
{
    std::ifstream file(path, std::ios::binary);

    if (!file)
    {
        const std::string message = "Couldn't find mesh to load with path: " + std::string(path);
        dvl::Log(dvl::LogLevel::Error, message.c_str());
        return {};
    }

    file.seekg(0, std::ios::end);

    const std::streamsize fileSize = file.tellg();
    if (fileSize < 0)
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to get file size during mesh loading!");
        return {};
    }

    std::vector<std::uint8_t> data(static_cast<size_t>(fileSize));

    file.seekg(0, std::ios::beg);

    if (!file.read(reinterpret_cast<char*>(data.data()), fileSize))
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to read file data during mesh loading!");
        return {};
    }

    // Load header
    if (data.size() < sizeof(dvl::MeshFileHeader))
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid mesh header!");
        return {};
    }

    dvl::MeshFileHeader header{};
    std::memcpy(&header, data.data(), sizeof(header));

    if (header.magic != dvl::MeshMagic || header.version != dvl::MeshVersion)
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to deserialize mesh!");
        return {};
    }

    // Load vertices
    std::vector<VertexPositionNormalUV> vertices(header.vertexCount);
    const std::uint8_t* vertexPtr = data.data() + sizeof(dvl::MeshFileHeader);
    std::memcpy(vertices.data(), vertexPtr, sizeof(dvl::MeshVertexFormat) * header.vertexCount);

    // Load indices
    std::vector<std::uint16_t> indices(header.indexCount);
    const std::uint8_t* indexPtr = data.data() + sizeof(dvl::MeshFileHeader) + header.vertexCount * sizeof(dvl::MeshVertexFormat);
    std::memcpy(indices.data(), indexPtr, sizeof(std::uint16_t) * header.indexCount);

    MeshDesc desc = {};
    desc.vertexData = vertices.data();
    desc.vertexDataSize = vertices.size() * sizeof(VertexPositionNormalUV);
    desc.indices = indices.data();
    desc.indexCount = indices.size();

    Mesh mesh = {};
    MeshHandle meshHandle = {};
    if (renderer.CreateMesh(desc, mesh))
    {
        meshHandle.id = _nextMeshId++;
        _meshes.emplace(meshHandle, mesh);

        const std::string message = "Loaded mesh successfully at path: " + std::string(path);
        dvl::Log(dvl::LogLevel::Info, message.c_str());
    }

    return meshHandle;
}

void AssetRegistry::UnloadMesh(const MeshHandle& meshHandle, Renderer& renderer)
{
    const auto it = _meshes.find(meshHandle);

    if (it == _meshes.end())
    {
        dvl::Log(dvl::LogLevel::Error, "Couldn't find mesh to unload!");
        return;
    }

    renderer.DestroyMesh(it->second);
    _meshes.erase(it);
}

dvl::TextureHandle AssetRegistry::LoadTexture(const std::filesystem::path& path, Renderer& renderer)
{
    std::ifstream file(path, std::ios::binary);

    if (!file)
    {
        const std::string message = "Couldn't find texture to load with path: " + std::string(path);
        dvl::Log(dvl::LogLevel::Error, message.c_str());
        return {};
    }

    file.seekg(0, std::ios::end);
    const std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<std::uint8_t> data(static_cast<std::size_t>(fileSize));
    if (!file.read(reinterpret_cast<char*>(data.data()), fileSize))
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to read file data during texture loading!");
        return {};
    }

    dvl::TextureFileHeader header{};
    std::memcpy(&header, data.data(), sizeof(header));

    if (header.magic != dvl::TextureMagic || header.version != dvl::TextureVersion)
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid texture header!");
        return {};
    }

    dvl::TextureDesc desc{};
    desc.width = header.width;
    desc.height = header.height;
    desc.data = data.data() + sizeof(dvl::TextureFileHeader);

    const dvl::TextureHandle texture = renderer.CreateTexture(desc);

    if (texture.IsValid())
    {
        _textures.push_back(texture);

        const std::string message = "Loaded texture successfully at path: " + std::string(path);
        dvl::Log(dvl::LogLevel::Info, message.c_str());
    }

    return texture;
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

const RenderPipeline* AssetRegistry::GetRenderPipeline(const RenderPipelineHandle& renderPipelineHandle) const
{
     const auto it = _pipelines.find(renderPipelineHandle);

    if (it != _pipelines.end())
    {
        return &it->second;
    }

    return nullptr;
}

dvl::TextureHandle AssetRegistry::GetDefaultTexture() const
{
    return _defaultTexture;
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

void AssetRegistry::loadCubePrimitive(Renderer& renderer)
{
    const VertexPositionNormalUV CubeVertices[] =
    {
        // Front
        {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f},
        { 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f},
        {-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f},
        { 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f},

        // Back
        { 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f},
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f},
        { 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f},
        {-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f},

        // Left
        {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f},
        {-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f},
        {-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f},
        {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f},

        // Right
        { 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f},
        { 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f},
        { 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f},
        { 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f},

        // Top
        {-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f},
        { 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f},
        {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f},
        { 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f},

        // Bottom
        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f},
        { 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f},
        {-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f},
        { 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f}
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

    if (renderer.CreateMesh(desc, cubeMesh))
    {
        _cubeMeshHandle.id = _nextMeshId++;
        _meshes.emplace(_cubeMeshHandle, cubeMesh);
    }
}

void AssetRegistry::loadDefaultTexture(Renderer& renderer)
{
    const std::uint8_t whitePixel[] = {255, 255, 255, 255};

    dvl::TextureDesc desc{};
    desc.width = 1;
    desc.height = 1;
    desc.data = whitePixel;

    _defaultTexture = renderer.CreateTexture(desc);
    if (_defaultTexture.IsValid())
    {
        _textures.push_back(_defaultTexture);
    }
}

void AssetRegistry::loadMaterials(Renderer& renderer)
{
    const dvl::VertexAttribute attributes[] =
    {
        {"aPosition", dvl::VertexFormat::Float3, offsetof(VertexPositionNormalUV, x)},
        {"aNormal", dvl::VertexFormat::Float3, offsetof(VertexPositionNormalUV, nx)},
        {"aUV", dvl::VertexFormat::Float2, offsetof(VertexPositionNormalUV, u)}
    };

    const ShaderParameterDesc parameters[] =
    {
        {"viewProjectionMatrix", dvl::ShaderParameterType::Mat4},
        {"modelMatrix", dvl::ShaderParameterType::Mat4},
        {"materialColor", dvl::ShaderParameterType::Float4},
        {"lightCount", dvl::ShaderParameterType::Int},
        {"lightDirections", dvl::ShaderParameterType::Float4},
        {"lightColors", dvl::ShaderParameterType::Float4},
        {"cameraPosition", dvl::ShaderParameterType::Float3},
        {"albedoTexture", dvl::ShaderParameterType::Int}
    };

    Material solidMaterial = {};
    Material wireframeMaterial = {};

    solidMaterial.textureHandle = _defaultTexture;
    wireframeMaterial.textureHandle = _defaultTexture;

    RenderPipelineDesc pipelineDesc = {};
    pipelineDesc.vertexShaderPath = "app0:/asset/shaders/vertex.vert";
    pipelineDesc.fragmentShaderPath = "app0:/asset/shaders/fragment.frag";
    pipelineDesc.attributes = attributes;
    pipelineDesc.attributeCount = sizeof(attributes) / sizeof(attributes[0]);
    pipelineDesc.vertexStride = sizeof(VertexPositionNormalUV);
    pipelineDesc.parameters = parameters;
    pipelineDesc.parameterCount = sizeof(parameters) / sizeof(parameters[0]);
    pipelineDesc.depthStencilState.depthTestEnabled = true;
    pipelineDesc.depthStencilState.depthWriteEnabled = true;

    RenderPipeline solidRenderPipeline = renderer.CreateRenderPipeline(pipelineDesc);
    pipelineDesc.rasterizerState.fillMode = dvl::FillMode::Wireframe;
    RenderPipeline wireframeRenderPipeline= renderer.CreateRenderPipeline(pipelineDesc);
    
    RenderPipelineHandle solidPipelineHandle = {};
    solidPipelineHandle.id = _nextPipelineId++;
    
    RenderPipelineHandle wireframePipelineHandle = {};
    wireframePipelineHandle.id = _nextPipelineId++;
    
    _pipelines.emplace(solidPipelineHandle, solidRenderPipeline);
    _pipelines.emplace(wireframePipelineHandle, wireframeRenderPipeline);
    
    solidMaterial.renderPipelineHandle = solidPipelineHandle;
    wireframeMaterial.renderPipelineHandle = wireframePipelineHandle;

    _solidMaterialHandle.id = _nextMaterialId++;
    _wireframeMaterialHandle.id = _nextMaterialId++;

    _materials.emplace(_solidMaterialHandle, solidMaterial);
    _materials.emplace(_wireframeMaterialHandle, wireframeMaterial);
}
