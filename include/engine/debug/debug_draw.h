#pragma once

#include <dvl/dvl.h>

#include "engine/render/material.h"
#include "engine/render/mesh.h"

class AssetRegistry;
class Renderer;

class DebugDraw
{
public:
    static void Initialize(const AssetRegistry& assetRegistry, Renderer* renderer);

    static void DrawLine(const dvl::Vec3& from, const dvl::Vec3& to, dvl::Vec4 color);

private:
    static Renderer* _renderer;
    
    static Mesh _lineMesh;
    static Material _material;
};
