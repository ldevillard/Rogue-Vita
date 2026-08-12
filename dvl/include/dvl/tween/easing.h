#pragma once

namespace dvl
{
    enum class Easing
    {
        Linear,

        InBack,
        OutBack,
        InOutBack,

        // ...
    };

    float ApplyEasing(Easing type, float t);
}