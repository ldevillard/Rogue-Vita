#pragma once

#include "dvl/asset/asset_helper.h"

namespace dvl
{
    constexpr std::uint32_t MeshMagic = MakeMagic('D', 'V', 'L', 'M');
    constexpr std::uint32_t MeshVersion = 2;

    enum class MeshType : std::uint32_t
    {
        Static,
        Skinned
    };

    // TODO: Added bounds min and bounds max
    struct MeshFileHeader
    {
        std::uint32_t magic;
        std::uint32_t version;
        std::uint32_t vertexCount;
        std::uint32_t indexCount;
        MeshType meshType;
    };

    struct MeshVertexFormat
    {
        float x, y, z;
        float nx, ny, nz;

        float u, v;

        std::uint8_t boneIndices[4];
        std::uint8_t boneWeights[4];
    };
}
