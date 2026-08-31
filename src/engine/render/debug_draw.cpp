#include "engine/render/debug_draw.h"

#include <cmath>
#include <cstdint>

#include "engine/core/asset_registry.h"
#include "engine/render/renderer.h"

DebugDraw::DebugDraw(const AssetRegistry& assetRegistry)
    : _cubeMesh(assetRegistry.GetCubeMesh())
    , _material(assetRegistry.GetSolidMaterialInstance())
{
}

void DebugDraw::DrawLine(Renderer& renderer, const dvl::Vec3& from, const dvl::Vec3& to)
{
    const dvl::Vec3 delta = to - from;
    if (delta.LengthSquared() == 0.0f)
    {
        return;
    }

    constexpr float LineThickness = 0.025f;

    const dvl::Vec3 direction = delta.Normalized();
    const dvl::Vec3 referenceAxis = std::abs(direction.y) < std::abs(direction.x) ? dvl::Vec3(0.0f, 1.0f, 0.0f) : dvl::Vec3(1.0f, 0.0f, 0.0f);

    const dvl::Vec3 side = dvl::Cross(referenceAxis, direction).Normalized() * LineThickness;
    const dvl::Vec3 depth = dvl::Cross(side, direction);
    const dvl::Vec3 midpoint = (from + to) * 0.5f;

    dvl::Mat4 matrix = dvl::Mat4::Identity();

    matrix[0][0] = side.x;
    matrix[0][1] = side.y;
    matrix[0][2] = side.z;

    matrix[1][0] = delta.x;
    matrix[1][1] = delta.y;
    matrix[1][2] = delta.z;

    matrix[2][0] = depth.x;
    matrix[2][1] = depth.y;
    matrix[2][2] = depth.z;

    matrix[3][0] = midpoint.x;
    matrix[3][1] = midpoint.y;
    matrix[3][2] = midpoint.z;

    renderer.Draw(_cubeMesh, _material, matrix);
}

void DebugDraw::DrawSkeleton(Renderer& renderer, const dvl::Skeleton& skeleton, const dvl::Mat4* worldPose, const dvl::Mat4& modelMatrix)
{
    if (worldPose == nullptr || skeleton.parents == nullptr || skeleton.boneCount <= 0)
    {
        return;
    }

    for (int i = 0; i < skeleton.boneCount; i++)
    {
        const std::int16_t parent = skeleton.parents[i];

        if (parent < 0 || parent >= skeleton.boneCount)
        {
            continue;
        }

        const dvl::Vec3 parentPosition(worldPose[parent][3][0], worldPose[parent][3][1], worldPose[parent][3][2]);

        const dvl::Vec3 bonePosition(worldPose[i][3][0], worldPose[i][3][1], worldPose[i][3][2]);

        const dvl::Vec3 parentWorldPosition = (modelMatrix * dvl::Vec4(parentPosition.x, parentPosition.y, parentPosition.z, 1.0f)).XYZ();
        const dvl::Vec3 boneWorldPosition = (modelMatrix * dvl::Vec4(bonePosition.x, bonePosition.y, bonePosition.z, 1.0f)).XYZ();

        DrawLine(renderer, parentWorldPosition, boneWorldPosition);
    }
}
