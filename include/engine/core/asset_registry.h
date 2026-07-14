#pragma once

#include <unordered_map>

#include "engine/render/material.h"
#include "engine/render/mesh.h"
#include "engine/render/renderer.h"

class AssetRegistry
{
public:
    AssetRegistry(Renderer& Renderer);

    const Mesh* GetMesh(const MeshHandle& meshHandle) const;
    const Material* GetMaterial(const MaterialHandle& materialHandle) const;

    // primitives
    const Mesh* GetCubeMesh() const;

    // materials
    const Material* GetSolidMaterial() const;
    const Material* GetWireframeMaterial() const;

private:
    void loadCubePrimitive();
    void loadMaterials();

    // primitives
    MeshHandle _cubeMeshHandle;

    // materials
    MaterialHandle _solidMaterialHandle;
    MaterialHandle _wireframeMaterialHandle;

    std::unordered_map<MeshHandle, Mesh, MeshHandle::Hasher> _meshes;
    int _nextMeshId = 1;

    // TODO: register pipelines in an unordered_map

    std::unordered_map<MaterialHandle, Material, MaterialHandle::Hasher> _materials;
    int _nextMaterialId = 1;

    Renderer& _renderer;
};