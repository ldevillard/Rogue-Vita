#pragma once

#include <dvl/dvl.h>

#include "material.h"
#include "mesh.h"

class AssetRegistry;
class Renderer;

class DebugDraw
{
public:
    DebugDraw(const AssetRegistry& assetRegistry);

    // TODO: Use a line of two vertices instead of a raw cube
    void DrawLine(Renderer& renderer, const dvl::Vec3& from, const dvl::Vec3& to);

    void DrawSkeleton(Renderer& renderer, const dvl::Skeleton& skeleton, const dvl::Mat4* worldPose, const dvl::Mat4& modelMatrix);

private:
    Mesh _cubeMesh;
    Material _material;
};
