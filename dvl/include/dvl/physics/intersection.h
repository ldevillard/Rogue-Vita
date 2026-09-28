#pragma once

#include "dvl/geometry/geometry.h"

namespace dvl
{
    bool Intersects(const Sphere& sphere, const Obb& box);
    bool Intersects(const Ray& ray, const Obb& box, float& outDistance, Vec3& outNormal);
}