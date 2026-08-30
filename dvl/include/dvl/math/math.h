#pragma once

#include "mat.h"
#include "quat.h"
#include "transform.h"
#include "vec.h"

namespace dvl
{
    constexpr float Pi = 3.14159265358979323846f;

    constexpr float Radians(float degrees)
    {
        return degrees * (Pi / 180.0f);
    }

    constexpr float Degrees(float radians)
    {
        return radians * (180.0f / Pi);
    }
}
