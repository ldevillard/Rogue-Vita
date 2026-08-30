#include "dvl/math/math.h"

#include <cmath>

namespace dvl
{
    float Radians(float degrees)
    {
        return degrees * (Pi / 180.0f);
    }

    float Degrees(float radians)
    {
        return radians * (180.0f / Pi);
    }

    float Clamp(float value, float minimum, float maximum)
    {
        return value < minimum ? minimum : (value > maximum ? maximum : value);
    }

    float InverseSqrt(float value)
    {
        return 1.0f / std::sqrt(value);
    }
}
