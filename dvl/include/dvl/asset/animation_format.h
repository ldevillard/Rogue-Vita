#pragma once

#include "dvl/asset/asset_helper.h"
#include "dvl/math/transform.h"

namespace dvl
{
    constexpr std::uint32_t AnimationMagic = MakeMagic('D', 'V', 'L', 'A');
    constexpr std::uint32_t AnimationVersion = 1;

    struct AnimationFileHeader
    {
        std::uint32_t magic;
        std::uint32_t version;

        float duration;
        float fps;

        std::uint32_t boneCount;
        std::uint32_t frameCount;
    };
}
