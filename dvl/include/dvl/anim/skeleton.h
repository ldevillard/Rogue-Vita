#pragma once

#include <cstdint>

namespace dvl
{
    struct Skeleton
    {
        int boneCount = 0;
        const std::int16_t* parents = nullptr;
        const Mat4* inverseBindMatrices = nullptr;
    };
}