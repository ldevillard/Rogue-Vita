#pragma once

#include "dvl/asset/asset_helper.h"

namespace dvl
{
    constexpr std::uint32_t TextureMagic = MakeMagic('D', 'V', 'L', 'T');
    constexpr std::uint32_t TextureVersion = 1;

    struct TextureFileHeader
    {
        std::uint32_t magic;
        std::uint32_t version;
        std::uint32_t width;
        std::uint32_t height;
    };
}
