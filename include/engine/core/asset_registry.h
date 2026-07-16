#pragma once

#include <unordered_map>

#include "engine/render/material.h"
#include "engine/render/mesh.h"

class Renderer;

class AssetRegistry
{
public:
    void Initialize(Renderer& renderer);
    void Shutdown(Renderer& renderer);

    // TODO: Load
    // TODO: Unload

    const Mesh* GetMesh(const MeshHandle& meshHandle) const;
    const Material* GetMaterial(const MaterialHandle& materialHandle) const;
    const RenderPipeline* GetRenderPipeline(const RenderPipelineHandle& renderPipelineHandle) const;

    // primitives
    const Mesh& GetCubeMesh() const;

    // materials
    const Material GetSolidMaterialInstance() const;
    const Material GetWireframeMaterialInstance() const;

private:
    void loadCubePrimitive(Renderer& renderer);
    void loadMaterials(Renderer& renderer);

    // primitives
    MeshHandle _cubeMeshHandle;

    // materials
    MaterialHandle _solidMaterialHandle;
    MaterialHandle _wireframeMaterialHandle;

    std::unordered_map<MeshHandle, Mesh, MeshHandle::Hasher> _meshes;
    int _nextMeshId = 1;

    std::unordered_map<RenderPipelineHandle, RenderPipeline, RenderPipelineHandle::Hasher> _pipelines;
    int _nextPipelineId = 1;

    std::unordered_map<MaterialHandle, Material, MaterialHandle::Hasher> _materials;
    int _nextMaterialId = 1;
};