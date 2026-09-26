#include "engine/debug/debug_draw.h"

#include "engine/core/asset_registry.h"
#include "engine/render/renderer.h"

Renderer* DebugDraw::_renderer = nullptr;
Mesh DebugDraw::_lineMesh = {};
Material DebugDraw::_material = {};

void DebugDraw::Initialize(const AssetRegistry& assetRegistry, Renderer* renderer)
{
    _lineMesh = assetRegistry.GetLineMesh();
    _material = assetRegistry.GetDebugMaterialInstance();
    _renderer = renderer;
}

void DebugDraw::DrawLine(const dvl::Vec3& from, const dvl::Vec3& to, dvl::Vec4 color)
{
    if (_renderer == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "DebugDraw is not initialized, draw call canceled!");
        return;
    }

    float length = (to - from).Length();
    dvl::Vec3 direction = (to - from).Normalized();

    const dvl::Mat4 mat = dvl::Mat4::Translation(from) * dvl::Mat4::LookRotation(direction) * dvl::Mat4::Scale(dvl::Vec3(1.0f, 1.0f, length));

    _material.color = color;

    _renderer->Draw(_lineMesh, _material, mat);
}
