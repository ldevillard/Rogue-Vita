#pragma once

namespace dvl
{
    enum class Easing
    {
        Linear,

        InSine,
        OutSine,
        InOutSine,

        InBack,
        OutBack,
        InOutBack,

        // ...
    };

    float ApplyEasing(Easing type, float t);
}