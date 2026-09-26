#include "engine/core/asset_registry.h"

#include <cstring>
#include <fstream>

#include <dvl/asset/animation_format.h>
#include <dvl/asset/mesh_format.h>
#include <dvl/asset/texture_format.h>
#include <dvl/asset/skeleton_format.h>

#include "engine/render/vertex.h"
#include "engine/render/render_pipeline.h"

#include "engine/render/primitive/cube_mesh_data.h"
#include "engine/render/primitive/line_mesh_data.h"
#include "engine/render/primitive/wire_cube_mesh_data.h"

void AssetRegistry::Initialize(Renderer& renderer)
{
    _lineMeshHandle = loadPrimitive<LineMeshData>(renderer);
    _cubeMeshHandle = loadPrimitive<CubeMeshData>(renderer);
    _wireCubeMeshHandle = loadPrimitive<WireCubeMeshData>(renderer);

    loadDefaultTexture(renderer);
    loadMaterials(renderer);
    loadSkinnedMaterials(renderer);
    loadDebugMaterial(renderer);
}

void AssetRegistry::Shutdown(Renderer& renderer)
{
    for (std::pair<const TextureHandle, Texture>& texturePair : _textures)
    {
        renderer.DestroyTexture(texturePair.second);
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

    const std::uint8_t* vertexPtr = data.data() + sizeof(dvl::MeshFileHeader);

    // Load indices
    std::vector<std::uint16_t> indices(header.indexCount);
    const std::uint8_t* indexPtr = data.data() + sizeof(dvl::MeshFileHeader) + header.vertexCount * sizeof(dvl::MeshVertexFormat);
    std::memcpy(indices.data(), indexPtr, sizeof(std::uint16_t) * header.indexCount);

    MeshDesc desc = {};
    desc.indices = indices.data();
    desc.indexCount = indices.size();

    Mesh mesh = {};
    MeshHandle meshHandle = {};
    if (header.meshType == dvl::MeshType::Skinned)
    {
        std::vector<SkinnedVertexPositionNormalUV> vertices(header.vertexCount);
        std::memcpy(vertices.data(), vertexPtr, sizeof(dvl::MeshVertexFormat) * header.vertexCount);

        desc.vertexData = vertices.data();
        desc.vertexDataSize = vertices.size() * sizeof(SkinnedVertexPositionNormalUV);
        renderer.CreateMesh(desc, mesh);
    }
    else
    {
        std::vector<VertexPositionNormalUV> vertices(header.vertexCount);
        std::vector<dvl::MeshVertexFormat> source(header.vertexCount);
        std::memcpy(source.data(), vertexPtr, sizeof(dvl::MeshVertexFormat) * header.vertexCount);

        for (unsigned int index = 0; index < header.vertexCount; ++index)
        {
            vertices[index] =
            {
                source[index].x, source[index].y, source[index].z,
                source[index].nx, source[index].ny, source[index].nz,
                source[index].u, source[index].v
            };
        }

        desc.vertexData = vertices.data();
        desc.vertexDataSize = vertices.size() * sizeof(VertexPositionNormalUV);
        renderer.CreateMesh(desc, mesh);
    }

    if (mesh.IsValid())
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

TextureHandle AssetRegistry::LoadTexture(const std::filesystem::path& path, Renderer& renderer)
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

    TextureDesc desc = {};
    desc.width = header.width;
    desc.height = header.height;
    desc.data = data.data() + sizeof(dvl::TextureFileHeader);

    Texture texture = {};
    TextureHandle textureHandle = {};
    if (renderer.CreateTexture(desc, texture))
    {
        textureHandle.id = _nextTextureId++;
        _textures.emplace(textureHandle, texture);

        const std::string message = "Loaded texture successfully at path: " + std::string(path);
        dvl::Log(dvl::LogLevel::Info, message.c_str());
    }

    return textureHandle;
}

void AssetRegistry::UnloadTexture(const TextureHandle& textureHandle, Renderer& renderer)
{
    const auto it = _textures.find(textureHandle);

    if (it == _textures.end())
    {
        dvl::Log(dvl::LogLevel::Error, "Couldn't find texture to unload!");
        return;
    }

    renderer.DestroyTexture(it->second);
    _textures.erase(it);
}

AnimationHandle AssetRegistry::LoadAnimation(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file)
    {
        const std::string message = "Couldn't find animation to load with path: " + std::string(path);
        dvl::Log(dvl::LogLevel::Error, message.c_str());
        return {};
    }

    file.seekg(0, std::ios::end);

    const std::streamsize fileSize = file.tellg();
    if (fileSize < 0)
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to get file size during animation loading!");
        return {};
    }

    std::vector<std::uint8_t> data(static_cast<size_t>(fileSize));

    file.seekg(0, std::ios::beg);

    if (!file.read(reinterpret_cast<char*>(data.data()), fileSize))
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to read file data during animation loading!");
        return {};
    }

    // Load header
    if (data.size() < sizeof(dvl::AnimationFileHeader))
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid animation header!");
        return {};
    }

    dvl::AnimationFileHeader header{};
    std::memcpy(&header, data.data(), sizeof(header));

    if (header.magic != dvl::AnimationMagic || header.version != dvl::AnimationVersion)
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to deserialize animation!");
        return {};
    }

    // Load keyframes
    std::vector<dvl::Transform> keyframes(static_cast<std::size_t>(header.boneCount) * header.frameCount);
    const std::uint8_t* keyframesPtr = data.data() + sizeof(dvl::AnimationFileHeader);
    std::memcpy(keyframes.data(), keyframesPtr, keyframes.size() * sizeof(dvl::Transform));

    Animation animation = {};
    animation.duration = header.duration;
    animation.fps = header.fps;
    animation.boneCount = static_cast<int>(header.boneCount);
    animation.frameCount = static_cast<int>(header.frameCount);
    animation.keyframes = std::move(keyframes);

    AnimationHandle animationHandle = {};
    animationHandle.id = _nextAnimationId++;

    _animations.emplace(animationHandle, std::move(animation));

    const std::string message = "Loaded animation successfully at path: " + std::string(path);
    dvl::Log(dvl::LogLevel::Info, message.c_str());

    return animationHandle;
}

void AssetRegistry::UnloadAnimation(const AnimationHandle& animationHandle)
{
    const auto it = _animations.find(animationHandle);

    if (it == _animations.end())
    {
        dvl::Log(dvl::LogLevel::Error, "Couldn't find animation to unload!");
        return;
    }

    _animations.erase(it);
}

SkeletonHandle AssetRegistry::LoadSkeleton(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file)
    {
        const std::string message = "Couldn't find skeleton to load with path: " + std::string(path);
        dvl::Log(dvl::LogLevel::Error, message.c_str());
        return {};
    }

    file.seekg(0, std::ios::end);

    const std::streamsize fileSize = file.tellg();
    if (fileSize < 0)
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to get file size during skeleton loading!");
        return {};
    }

    std::vector<std::uint8_t> data(static_cast<size_t>(fileSize));

    file.seekg(0, std::ios::beg);

    if (!file.read(reinterpret_cast<char*>(data.data()), fileSize))
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to read file data during skeleton loading!");
        return {};
    }

    // Load header
    if (data.size() < sizeof(dvl::SkeletonFileHeader))
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid skeleton header!");
        return {};
    }

    dvl::SkeletonFileHeader header{};
    std::memcpy(&header, data.data(), sizeof(header));

    if (header.magic != dvl::SkeletonMagic || header.version != dvl::SkeletonVersion)
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to deserialize skeleton!");
        return {};
    }

    // Load parents
    std::vector<std::int16_t> parents(header.boneCount);
    const std::uint8_t* parentsPtr = data.data() + sizeof(dvl::SkeletonFileHeader);
    std::memcpy(parents.data(), parentsPtr, sizeof(std::int16_t) * header.boneCount);

    // Load inverse bind matrices
    std::vector<dvl::Mat4> inverseBindMatrices(header.boneCount);
    const std::uint8_t* matricesPtr = data.data() + sizeof(dvl::SkeletonFileHeader) + header.boneCount * sizeof(std::int16_t);
    std::memcpy(inverseBindMatrices.data(), matricesPtr, sizeof(dvl::Mat4) * header.boneCount);

    Skeleton skeleton = {};
    skeleton.parents = std::move(parents);
    skeleton.inverseBindMatrices = std::move(inverseBindMatrices);
    skeleton.boneCount = header.boneCount;

    SkeletonHandle skeletonHandle = {};
    skeletonHandle.id = _nextSkeletonId++;

    _skeletons.emplace(skeletonHandle, std::move(skeleton));

    const std::string message = "Loaded skeleton successfully at path: " + std::string(path);
    dvl::Log(dvl::LogLevel::Info, message.c_str());

    return skeletonHandle;
}

void AssetRegistry::UnloadSkeleton(const SkeletonHandle& skeletonHandle)
{
    const auto it = _skeletons.find(skeletonHandle);

    if (it == _skeletons.end())
    {
        dvl::Log(dvl::LogLevel::Error, "Couldn't find skeleton to unload!");
        return;
    }

    _skeletons.erase(it);
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

const Texture* AssetRegistry::GetTexture(const TextureHandle& textureHandle) const
{
    const auto it = _textures.find(textureHandle);

    if (it != _textures.end())
    {
        return &it->second;
    }

    return nullptr;
}

const Skeleton* AssetRegistry::GetSkeleton(const SkeletonHandle& skeletonHandle) const
{
    const auto it = _skeletons.find(skeletonHandle);

    if (it != _skeletons.end())
    {
        return &it->second;
    }

    return nullptr;
}

const Animation* AssetRegistry::GetAnimation(const AnimationHandle& animationHandle) const
{
    const auto it = _animations.find(animationHandle);

    if (it != _animations.end())
    {
        return &it->second;
    }

    return nullptr;
}

const Texture& AssetRegistry::GetDefaultTexture() const
{
    return _textures.at(_defaultTextureHandle);
}

const Mesh& AssetRegistry::GetLineMesh() const
{
    return _meshes.at(_lineMeshHandle);
}

const Mesh& AssetRegistry::GetCubeMesh() const
{
    return _meshes.at(_cubeMeshHandle);
}

const Mesh& AssetRegistry::GetWireCubeMesh() const
{
    return _meshes.at(_wireCubeMeshHandle);
}

const Material AssetRegistry::GetSolidMaterialInstance() const
{
    return _materials.at(_solidMaterialHandle);
}

const Material AssetRegistry::GetWireframeMaterialInstance() const
{
    return _materials.at(_wireframeMaterialHandle);
}

const Material AssetRegistry::GetDebugMaterialInstance() const
{
    return _materials.at(_debugMaterialHandle);
}

void AssetRegistry::loadDefaultTexture(Renderer& renderer)
{
    const std::uint8_t whitePixel[] = {255, 255, 255, 255};

    TextureDesc desc = {};
    desc.width = 1;
    desc.height = 1;
    desc.data = whitePixel;

    Texture defaultTexture = {};
    if (renderer.CreateTexture(desc, defaultTexture))
    {
        _defaultTextureHandle.id = _nextTextureId++;
        _textures.emplace(_defaultTextureHandle, defaultTexture);
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
        {"viewProjectionMatrix", dvl::ShaderParameterType::Mat4, ShaderParameterSemantic::ViewProjection},
        {"modelMatrix", dvl::ShaderParameterType::Mat4, ShaderParameterSemantic::ModelMatrix},
        {"materialColor", dvl::ShaderParameterType::Float4, ShaderParameterSemantic::MaterialColor},
        {"lightCount", dvl::ShaderParameterType::Int, ShaderParameterSemantic::LightCount},
        {"lightDirections", dvl::ShaderParameterType::Float4, ShaderParameterSemantic::LightDirections},
        {"lightColors", dvl::ShaderParameterType::Float4, ShaderParameterSemantic::LightColors},
        {"cameraPosition", dvl::ShaderParameterType::Float3, ShaderParameterSemantic::CameraPosition},
        {"albedoTexture", dvl::ShaderParameterType::Int, ShaderParameterSemantic::AlbedoTexture}
    };

    Material solidMaterial = {};
    Material wireframeMaterial = {};

    solidMaterial.textureHandle = _defaultTextureHandle;
    wireframeMaterial.textureHandle = _defaultTextureHandle;

    RenderPipelineDesc pipelineDesc = {};
    pipelineDesc.vertexShaderPath = dvl::Filesystem::GetAssetPath("shader/vertex.vert");
    pipelineDesc.fragmentShaderPath = dvl::Filesystem::GetAssetPath("shader/fragment.frag");
    pipelineDesc.attributes = attributes;
    pipelineDesc.attributeCount = sizeof(attributes) / sizeof(attributes[0]);
    pipelineDesc.vertexStride = sizeof(VertexPositionNormalUV);
    pipelineDesc.parameters = parameters;
    pipelineDesc.parameterCount = sizeof(parameters) / sizeof(parameters[0]);
    pipelineDesc.depthStencilState.depthTestEnabled = true;
    pipelineDesc.depthStencilState.depthWriteEnabled = true;

    RenderPipeline solidRenderPipeline = {};
    renderer.CreateRenderPipeline(pipelineDesc, solidRenderPipeline);

    pipelineDesc.rasterizerState.fillMode = dvl::FillMode::Wireframe;
    RenderPipeline wireframeRenderPipeline = {};
    renderer.CreateRenderPipeline(pipelineDesc, wireframeRenderPipeline);
    
    RenderPipelineHandle solidPipelineHandle = {};
    solidPipelineHandle.id = _nextPipelineId++;
    
    RenderPipelineHandle wireframePipelineHandle = {};
    wireframePipelineHandle.id = _nextPipelineId++;
    
    _pipelines.emplace(solidPipelineHandle, solidRenderPipeline);
    _pipelines.emplace(wireframePipelineHandle, wireframeRenderPipeline);

    solidMaterial.materialTemplate.staticPipeline = solidPipelineHandle;
    wireframeMaterial.materialTemplate.staticPipeline = wireframePipelineHandle;

    _solidMaterialHandle.id = _nextMaterialId++;
    _wireframeMaterialHandle.id = _nextMaterialId++;

    _materials.emplace(_solidMaterialHandle, solidMaterial);
    _materials.emplace(_wireframeMaterialHandle, wireframeMaterial);
}

void AssetRegistry::loadSkinnedMaterials(Renderer& renderer)
{
    const dvl::VertexAttribute attributes[] =
    {
        {"aPosition", dvl::VertexFormat::Float3, offsetof(SkinnedVertexPositionNormalUV, x)},
        {"aNormal", dvl::VertexFormat::Float3, offsetof(SkinnedVertexPositionNormalUV, nx)},
        {"aUV", dvl::VertexFormat::Float2, offsetof(SkinnedVertexPositionNormalUV, u)},
        {"aBoneIndices", dvl::VertexFormat::UByte4, offsetof(SkinnedVertexPositionNormalUV, boneIndices)},
        {"aBoneWeights", dvl::VertexFormat::UNormByte4, offsetof(SkinnedVertexPositionNormalUV, boneWeights)}
    };

    const ShaderParameterDesc parameters[] =
    {
        {"viewProjectionMatrix", dvl::ShaderParameterType::Mat4, ShaderParameterSemantic::ViewProjection},
        {"modelMatrix", dvl::ShaderParameterType::Mat4, ShaderParameterSemantic::ModelMatrix},
        {"materialColor", dvl::ShaderParameterType::Float4, ShaderParameterSemantic::MaterialColor},
        {"lightCount", dvl::ShaderParameterType::Int, ShaderParameterSemantic::LightCount},
        {"lightDirections", dvl::ShaderParameterType::Float4, ShaderParameterSemantic::LightDirections},
        {"lightColors", dvl::ShaderParameterType::Float4, ShaderParameterSemantic::LightColors},
        {"cameraPosition", dvl::ShaderParameterType::Float3, ShaderParameterSemantic::CameraPosition},
        {"albedoTexture", dvl::ShaderParameterType::Int, ShaderParameterSemantic::AlbedoTexture},
        {"skinningMatrices", dvl::ShaderParameterType::Float4, ShaderParameterSemantic::SkinningMatrices}
    };

    RenderPipelineDesc pipelineDesc = {};
    pipelineDesc.vertexShaderPath = dvl::Filesystem::GetAssetPath("shader/skinned_vertex.vert");
    pipelineDesc.fragmentShaderPath = dvl::Filesystem::GetAssetPath("shader/fragment.frag");
    pipelineDesc.attributes = attributes;
    pipelineDesc.attributeCount = sizeof(attributes) / sizeof(attributes[0]);
    pipelineDesc.vertexStride = sizeof(SkinnedVertexPositionNormalUV);
    pipelineDesc.parameters = parameters;
    pipelineDesc.parameterCount = sizeof(parameters) / sizeof(parameters[0]);
    pipelineDesc.depthStencilState.depthTestEnabled = true;
    pipelineDesc.depthStencilState.depthWriteEnabled = true;

    RenderPipeline skinnedSolidRenderPipeline = {};
    renderer.CreateRenderPipeline(pipelineDesc, skinnedSolidRenderPipeline);

    pipelineDesc.rasterizerState.fillMode = dvl::FillMode::Wireframe;
    RenderPipeline skinnedWireframeRenderPipeline = {};
    renderer.CreateRenderPipeline(pipelineDesc, skinnedWireframeRenderPipeline);

    RenderPipelineHandle skinnedSolidPipelineHandle = {};
    skinnedSolidPipelineHandle.id = _nextPipelineId++;

    RenderPipelineHandle skinnedWireframePipelineHandle = {};
    skinnedWireframePipelineHandle.id = _nextPipelineId++;

    _pipelines.emplace(skinnedSolidPipelineHandle, skinnedSolidRenderPipeline);
    _pipelines.emplace(skinnedWireframePipelineHandle, skinnedWireframeRenderPipeline);

    _materials.at(_solidMaterialHandle).materialTemplate.skinnedPipeline = skinnedSolidPipelineHandle;
    _materials.at(_wireframeMaterialHandle).materialTemplate.skinnedPipeline = skinnedWireframePipelineHandle;
}

void AssetRegistry::loadDebugMaterial(Renderer& renderer)
{
    const dvl::VertexAttribute attributes[] =
    {
        {"aPosition", dvl::VertexFormat::Float3, offsetof(VertexPositionColor, x)},
    };

    const ShaderParameterDesc parameters[] =
    {
        {"viewProjectionMatrix", dvl::ShaderParameterType::Mat4, ShaderParameterSemantic::ViewProjection},
        {"modelMatrix", dvl::ShaderParameterType::Mat4, ShaderParameterSemantic::ModelMatrix},
        {"materialColor", dvl::ShaderParameterType::Float4, ShaderParameterSemantic::MaterialColor},
    };

    RenderPipelineDesc pipelineDesc = {};
    pipelineDesc.vertexShaderPath = dvl::Filesystem::GetAssetPath("shader/debug/debug_vertex.vert");
    pipelineDesc.fragmentShaderPath = dvl::Filesystem::GetAssetPath("shader/debug/debug_fragment.frag");
    pipelineDesc.attributes = attributes;
    pipelineDesc.attributeCount = sizeof(attributes) / sizeof(attributes[0]);
    pipelineDesc.vertexStride = sizeof(VertexPosition);
    pipelineDesc.parameters = parameters;
    pipelineDesc.parameterCount = sizeof(parameters) / sizeof(parameters[0]);
    pipelineDesc.depthStencilState.depthTestEnabled = true;
    pipelineDesc.depthStencilState.depthWriteEnabled = true;
    pipelineDesc.topology = dvl::PrimitiveTopology::LineList;

    RenderPipeline debugRenderPipeline = {};
    renderer.CreateRenderPipeline(pipelineDesc, debugRenderPipeline);

    RenderPipelineHandle debugPipelineHandle = {};
    debugPipelineHandle.id = _nextPipelineId++;

    _pipelines.emplace(debugPipelineHandle, debugRenderPipeline);

    Material debugLineMaterial = {};
    debugLineMaterial.materialTemplate.staticPipeline = debugPipelineHandle;

    _debugMaterialHandle.id = _nextMaterialId++;
    _materials.emplace(_debugMaterialHandle, debugLineMaterial);
}