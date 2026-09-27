#pragma once

#include "dvl/geometry/geometry.h"

namespace dvl
{
    bool Intersects(const Sphere& sphere, const Obb& box);
}