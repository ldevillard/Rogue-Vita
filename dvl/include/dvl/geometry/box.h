#pragma once

#include "dvl/math/math.h"

namespace dvl
{
    struct Box
    {
        Vec3 center;
        Vec3 size;
    };

    struct Obb
    {
        Vec3 center;
        Vec3 halfExtents;
        Quat rotation;
    };
}