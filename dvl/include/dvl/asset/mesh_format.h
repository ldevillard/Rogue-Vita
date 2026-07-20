#pragma once

#include "dvl/asset/asset_helper.h"

namespace dvl
{
    constexpr std::uint32_t MeshMagic = MakeMagic('D', 'V', 'L', 'M');
    constexpr std::uint32_t MeshVersion = 1;

    struct MeshFileHeader
    {
        std::uint32_t magic;
        std::uint32_t version;
        std::uint32_t vertexCount;
        std::uint32_t indexCount;
    };

    struct MeshVertexFormat
    {
        float x, y, z;
        float nx, ny, nz;

        float u, v;
    };
}
