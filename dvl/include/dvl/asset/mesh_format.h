#pragma once

#include <cstdint>

namespace dvl
{
    constexpr std::uint32_t MakeMagic(char a, char b, char c, char d)
    {
        return (static_cast<std::uint32_t>(a) << 0)  |
               (static_cast<std::uint32_t>(b) << 8)  |
               (static_cast<std::uint32_t>(c) << 16) |
               (static_cast<std::uint32_t>(d) << 24);
    }

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
