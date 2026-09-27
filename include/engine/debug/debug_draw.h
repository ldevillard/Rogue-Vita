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

    static void DrawLine(const dvl::Vec3& from, const dvl::Vec3& to, const dvl::Vec4& color);
    static void DrawWireCube(const dvl::Vec3& position, const dvl::Vec3& size, const dvl::Vec4& color, const dvl::Mat4& parentTransform = dvl::Mat4::Identity());
    static void DrawWireSphere(const dvl::Vec3& position, float radius, const dvl::Vec4& color, const dvl::Mat4& parentTransform = dvl::Mat4::Identity());
    static void DrawCircle(const dvl::Vec3& position, float radius, const dvl::Vec4& color, const dvl::Mat4& parentTransform = dvl::Mat4::Identity());

private:
    static Renderer* _renderer;
    
    static Mesh _lineMesh;
    static Mesh _wireCubeMesh;
    static Mesh _wireSphereMesh;
    static Mesh _circleMesh;

    static Material _material;
};
