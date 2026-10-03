#include "engine/debug/debug_draw.h"

#include "engine/core/asset_registry.h"
#include "engine/render/renderer.h"

Renderer* DebugDraw::_renderer = nullptr;

const Mesh* DebugDraw::_lineMesh = {};
const Mesh* DebugDraw::_wireCubeMesh = {};
const Mesh* DebugDraw::_wireSphereMesh = {};
const Mesh* DebugDraw::_circleMesh = {};

Material DebugDraw::_material = {};

void DebugDraw::Initialize(AssetRegistry& assetRegistry, Renderer* renderer)
{
    _renderer = renderer;

    _lineMesh = assetRegistry.GetMesh(assetRegistry.GetLineMeshHandle());
    _wireCubeMesh = assetRegistry.GetMesh(assetRegistry.GetWireCubeMeshHandle());
    _wireSphereMesh = assetRegistry.GetMesh(assetRegistry.GetWireSphereMeshHandle());
    _circleMesh = assetRegistry.GetMesh(assetRegistry.GetCircleMeshHandle());

    _material = assetRegistry.GetDebugMaterialInstance();
}

void DebugDraw::DrawLine(const dvl::Vec3& from, const dvl::Vec3& to, const dvl::Vec4& color)
{
    if (GizmosEnabled == false)
        return;

    if (_renderer == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "DebugDraw is not initialized, draw call canceled!");
        return;
    }

    float length = (to - from).Length();
    dvl::Vec3 direction = (to - from).Normalized();
    const dvl::Vec3 up = direction.y > 0.999f || direction.y < -0.999f ? dvl::Vec3(0.0f, 0.0f, 1.0f) : dvl::Vec3(0.0f, 1.0f, 0.0f);

    const dvl::Mat4 mat = dvl::Mat4::Translation(from) * dvl::Mat4::LookRotation(direction, up) * dvl::Mat4::Scale(dvl::Vec3(1.0f, 1.0f, length));

    _material.color = color;

    _renderer->Draw(*_lineMesh, _material, mat);
}

void DebugDraw::DrawWireCube(const dvl::Vec3& position, const dvl::Vec3& size, const dvl::Vec4& color, const dvl::Mat4& parentTransform)
{
    if (GizmosEnabled == false)
        return;

    if (_renderer == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "DebugDraw is not initialized, draw call canceled!");
        return;
    }

    const dvl::Mat4 mat = parentTransform * dvl::Mat4::Translation(position) * dvl::Mat4::Scale(size);
    
    _material.color = color;

    _renderer->Draw(*_wireCubeMesh, _material, mat);
}

void DebugDraw::DrawWireSphere(const dvl::Vec3& position, float radius, const dvl::Vec4& color, const dvl::Mat4& parentTransform)
{
    if (GizmosEnabled == false)
        return;

    if (_renderer == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "DebugDraw is not initialized, draw call canceled!");
        return;
    }

    const float diameter = radius * 2.0f;
    const dvl::Mat4 mat = parentTransform * dvl::Mat4::Translation(position) * dvl::Mat4::Scale(dvl::Vec3(diameter, diameter, diameter));

    _material.color = color;

    _renderer->Draw(*_wireSphereMesh, _material, mat);
}

void DebugDraw::DrawCircle(const dvl::Vec3& position, float radius, const dvl::Vec4& color, const dvl::Mat4& parentTransform)
{
    if (GizmosEnabled == false)
        return;

    if (_renderer == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "DebugDraw is not initialized, draw call canceled!");
        return;
    }

    const float diameter = radius * 2.0f;
    const dvl::Mat4 mat = parentTransform * dvl::Mat4::Translation(position) * dvl::Mat4::Scale(dvl::Vec3(diameter, diameter, diameter));

    _material.color = color;

    _renderer->Draw(*_circleMesh, _material, mat);
}
