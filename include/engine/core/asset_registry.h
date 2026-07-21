#pragma once

#include <filesystem>
#include <unordered_map>

#include "engine/render/material.h"
#include "engine/render/mesh.h"
#include "engine/render/texture.h"

class Renderer;

class AssetRegistry
{
public:
    void Initialize(Renderer& renderer);
    void Shutdown(Renderer& renderer);

    MeshHandle LoadMesh(const std::filesystem::path& path, Renderer& renderer);
    void UnloadMesh(const MeshHandle& meshHandle, Renderer& renderer);

    TextureHandle LoadTexture(const std::filesystem::path& path, Renderer& renderer);
    void UnloadTexture(const TextureHandle& textureHandle, Renderer& renderer);

    // TODO: Load and unload materials

    const Mesh* GetMesh(const MeshHandle& meshHandle) const;
    const Material* GetMaterial(const MaterialHandle& materialHandle) const;
    const RenderPipeline* GetRenderPipeline(const RenderPipelineHandle& renderPipelineHandle) const;
    const Texture* GetTexture(const TextureHandle& textureHandle) const;

    // Primitives
    const Mesh& GetCubeMesh() const;

    // Materials
    const Material GetSolidMaterialInstance() const;
    const Material GetWireframeMaterialInstance() const;

    // Textures
    const Texture& GetDefaultTexture() const;

private:
    void loadCubePrimitive(Renderer& renderer);
    void loadDefaultTexture(Renderer& renderer);
    void loadMaterials(Renderer& renderer);

    // Primitives
    MeshHandle _cubeMeshHandle;

    // Materials
    MaterialHandle _solidMaterialHandle;
    MaterialHandle _wireframeMaterialHandle;

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
};
