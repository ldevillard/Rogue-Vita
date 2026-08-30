#pragma once

#include "mat.h"
#include "quat.h"
#include "transform.h"
#include "vec.h"

namespace dvl
{
    constexpr float Pi = 3.14159265358979323846f;

    float Radians(float degrees);
    float Degrees(float radians);
    float Clamp(float value, float minimum, float maximum);
    float InverseSqrt(float value);
}
