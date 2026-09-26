#pragma once

#include <filesystem>
#include <unordered_map>

#include "engine/render/animation.h"
#include "engine/render/material.h"
#include "engine/render/mesh.h"
#include "engine/render/renderer.h"
#include "engine/render/skeleton.h"
#include "engine/render/texture.h"

class AssetRegistry
{
public:
    void Initialize(Renderer& renderer);
    void Shutdown(Renderer& renderer);

    MeshHandle LoadMesh(const std::filesystem::path& path, Renderer& renderer);
    void UnloadMesh(const MeshHandle& meshHandle, Renderer& renderer);

    TextureHandle LoadTexture(const std::filesystem::path& path, Renderer& renderer);
    void UnloadTexture(const TextureHandle& textureHandle, Renderer& renderer);

    SkeletonHandle LoadSkeleton(const std::filesystem::path& path);
    void UnloadSkeleton(const SkeletonHandle& skeletonHandle);

    AnimationHandle LoadAnimation(const std::filesystem::path& path);
    void UnloadAnimation(const AnimationHandle& animationHandle);

    // TODO: Load and unload materials

    const Mesh* GetMesh(const MeshHandle& meshHandle) const;
    const Material* GetMaterial(const MaterialHandle& materialHandle) const;
    const RenderPipeline* GetRenderPipeline(const RenderPipelineHandle& renderPipelineHandle) const;
    const Texture* GetTexture(const TextureHandle& textureHandle) const;
    const Skeleton* GetSkeleton(const SkeletonHandle& skeletonHandle) const;
    const Animation* GetAnimation(const AnimationHandle& animationHandle) const;

    // Primitives
    const Mesh& GetCubeMesh() const;
    const Mesh& GetLineMesh() const;

    // Materials
    const Material GetSolidMaterialInstance() const;
    const Material GetWireframeMaterialInstance() const;
    const Material GetDebugMaterialInstance() const;

    // Textures
    const Texture& GetDefaultTexture() const;

private:
    template<typename MeshData>
    MeshHandle loadPrimitive(Renderer& renderer)
    {
        MeshDesc desc = {};
        desc.vertexData = MeshData::vertices;
        desc.vertexDataSize = sizeof(MeshData::vertices);
        desc.indices = MeshData::indices;
        desc.indexCount = sizeof( MeshData::indices) / sizeof(MeshData::indices[0]);

        Mesh mesh = {};
        MeshHandle meshHandle = {};

        if (renderer.CreateMesh(desc, mesh))
        {
            meshHandle.id = _nextMeshId++;
            _meshes.emplace(meshHandle, mesh);
        }

        return meshHandle;
    }

    void loadDefaultTexture(Renderer& renderer);

    void loadMaterials(Renderer& renderer);
    void loadSkinnedMaterials(Renderer& renderer);
    void loadDebugMaterial(Renderer& renderer);

    // Primitives
    MeshHandle _cubeMeshHandle;
    MeshHandle _lineMeshHandle;

    // Materials
    MaterialHandle _solidMaterialHandle;
    MaterialHandle _wireframeMaterialHandle;
    MaterialHandle _debugMaterialHandle;

    // Textures
    TextureHandle _defaultTextureHandle;

    std::unordered_map<MeshHandle, Mesh, MeshHandle::Hasher> _meshes;
    int _nextMeshId = 1;

    std::unordered_map<RenderPipelineHandle, RenderPipeline, RenderPipelineHandle::Hasher> _pipelines;
    int _nextPipelineId = 1;

    std::unordered_map<MaterialHandle, Material, MaterialHandle::Hasher> _materials;
    int _nextMaterialId = 1;

    std::unordered_map<TextureHandle, Texture, TextureHandle::Hasher> _textures;
    int _nextTextureId = 1;

    std::unordered_map<SkeletonHandle, Skeleton, SkeletonHandle::Hasher> _skeletons;
    int _nextSkeletonId = 1;

    std::unordered_map<AnimationHandle, Animation, AnimationHandle::Hasher> _animations;
    int _nextAnimationId = 1;
};
