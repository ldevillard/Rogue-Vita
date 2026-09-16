#pragma once

#include "dvl/asset/asset_helper.h"

namespace dvl
{
    constexpr std::uint32_t SkeletonMagic = MakeMagic('D', 'V', 'L', 'S');
    constexpr std::uint32_t SkeletonVersion = 2;

    struct SkeletonFileHeader
    {
        std::uint32_t magic;
        std::uint32_t version;
        std::uint32_t boneCount;
    };
}
