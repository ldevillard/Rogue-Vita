#pragma once

#include "dvl/input/types.h"

namespace dvl
{
    class Input
    {
    public:
        bool Initialize();
        void Shutdown();

        void Update();
    
        const GamepadState& GetState() const;
    
        bool IsButtonHeld(GamepadButton button) const;
        bool IsButtonDown(GamepadButton button) const;
        bool IsButtonUp(GamepadButton button) const;

    private:
        GamepadState _state {};
    };
}