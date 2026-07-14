#pragma once

#include <cstdint>

namespace dvl
{
    enum class GamepadButton : uint32_t
    {
        Cross    = 1u << 0,
        Circle   = 1u << 1,
        Square   = 1u << 2,
        Triangle = 1u << 3,

        Up       = 1u << 4,
        Down     = 1u << 5,
        Left     = 1u << 6,
        Right    = 1u << 7,

        L        = 1u << 8,
        R        = 1u << 9,

        Start    = 1u << 10,
        Select   = 1u << 11
    };

    // states between 0 and 1
    struct StickState
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct GamepadState
    {
        StickState leftStick;
        StickState rightStick;

        uint32_t buttons = 0;
        uint32_t previousButtons = 0;
    };
}