#pragma once

#include "dvl/input/types.h"

namespace dvl
{
    class Input
    {
    public:
        static bool Initialize();
        static void Shutdown();

        static void Update();
    
        static const GamepadState& GetState();
    
        static bool IsButtonHeld(GamepadButton button);
        static bool IsButtonDown(GamepadButton button);
        static bool IsButtonUp(GamepadButton button);

    private:
        static GamepadState _state;
    };
}
