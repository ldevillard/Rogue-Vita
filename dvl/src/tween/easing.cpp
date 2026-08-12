#include "dvl/tween/easing.h"

#include <cmath>

namespace
{
    // TODO: Add PI in a dvl math utility header
    constexpr float Pi = 3.14159265358979323846f;
    constexpr float HalfPi = Pi * 0.5f;

    constexpr float BackStrength = 1.70158f;
    constexpr float BackInOutStrengthMultiplier = 1.525f;
    constexpr float BackInOutStrength = BackStrength * BackInOutStrengthMultiplier;

    float EaseInSine(float time)
    {
        return 1.0f - std::cos(time * HalfPi);
    }

    float EaseOutSine(float time)
    {
        return std::sin(time * HalfPi);
    }

    float EaseInOutSine(float time)
    {
        return (1.0f - std::cos(time * Pi)) * 0.5f;
    }

    float EaseInBack(float time)
    {
        return time * time * ((BackStrength + 1.0f) * time - BackStrength);
    }

    float EaseOutBack(float time)
    {
        const float shiftedTime = time - 1.0f;

        return shiftedTime * shiftedTime * ((BackStrength + 1.0f) * shiftedTime + BackStrength) + 1.0f;
    }

    float EaseInOutBack(float time)
    {
        const float scaledTime = time * 2.0f;

        if (scaledTime < 1.0f)
        {
            return 0.5f * scaledTime * scaledTime * ((BackInOutStrength + 1.0f) * scaledTime - BackInOutStrength);
        }

        const float shiftedTime = scaledTime - 2.0f;

        return 0.5f * (shiftedTime * shiftedTime * ((BackInOutStrength + 1.0f) * shiftedTime + BackInOutStrength) + 2.0f);
    }
}

namespace dvl
{
    float ApplyEasing(Easing type, float time)
    {
        switch (type)
        {
        case Easing::Linear:
            return time;

        case Easing::InSine:
            return EaseInSine(time);

        case Easing::OutSine:
            return EaseOutSine(time);

        case Easing::InOutSine:
            return EaseInOutSine(time);

        case Easing::InBack:
            return EaseInBack(time);

        case Easing::OutBack:
            return EaseOutBack(time);

        case Easing::InOutBack:
            return EaseInOutBack(time);

        default:
            return time;
        }
    }
}
