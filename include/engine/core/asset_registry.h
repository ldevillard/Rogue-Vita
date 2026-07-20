#pragma once

#include <filesystem>
#include <unordered_map>

#include "engine/render/material.h"
#include "engine/render/mesh.h"

class Renderer;

class AssetRegistry
{
public:
    void Initialize(Renderer& renderer);
    void Shutdown(Renderer& renderer);

    MeshHandle LoadMesh(const std::filesystem::path& path, Renderer& renderer);
    void UnloadMesh(const MeshHandle& meshHandle, Renderer& renderer);

    // TODO: Use engine side handle instead of dvl one
    dvl::TextureHandle LoadTexture(const std::filesystem::path& path, Renderer& renderer);

    // TODO: Load and unload materials

    const Mesh* GetMesh(const MeshHandle& meshHandle) const;
    const Material* GetMaterial(const MaterialHandle& materialHandle) const;
    const RenderPipeline* GetRenderPipeline(const RenderPipelineHandle& renderPipelineHandle) const;
    dvl::TextureHandle GetDefaultTexture() const;

    // Primitives
    const Mesh& GetCubeMesh() const;

    // Materials
    const Material GetSolidMaterialInstance() const;
    const Material GetWireframeMaterialInstance() const;

private:
    void loadCubePrimitive(Renderer& renderer);
    void loadDefaultTexture(Renderer& renderer);
    void loadMaterials(Renderer& renderer);

    // Primitives
    MeshHandle _cubeMeshHandle;

    // Materials
    MaterialHandle _solidMaterialHandle;
    MaterialHandle _wireframeMaterialHandle;

    std::unordered_map<MeshHandle, Mesh, MeshHandle::Hasher> _meshes;
    int _nextMeshId = 1;

    std::unordered_map<RenderPipelineHandle, RenderPipeline, RenderPipelineHandle::Hasher> _pipelines;
    int _nextPipelineId = 1;

    std::unordered_map<MaterialHandle, Material, MaterialHandle::Hasher> _materials;
    int _nextMaterialId = 1;

    // TODO: Engine side texture type, use a map here
    std::vector<dvl::TextureHandle> _textures;
    dvl::TextureHandle _defaultTexture;
};
