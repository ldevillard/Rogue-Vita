#pragma once

#include "quat.h"
#include "vec.h"

namespace dvl
{
    // align on 16-byte for future SIMD optimizations
    struct alignas(16) Transform
    {
        Quat rotation = Quat::Identity();
        Vec4 translation = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
        Vec4 scale = Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    };
}
