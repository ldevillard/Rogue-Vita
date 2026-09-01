#include "dvl/input/input.h"

#include <algorithm>

namespace dvl
{
    GamepadState Input::_state {};

    std::uint32_t ToMask(GamepadButton button)
    {
        return static_cast<std::uint32_t>(button);
    }

    float NormalizeStick(std::uint8_t value)
    {
        constexpr float Center = 128.0f;
        constexpr float Range = 127.0f;
        constexpr float DeadZone = 0.15f;

        float result = (static_cast<float>(value) - Center) / Range;

        result = std::clamp(result, -1.0f, 1.0f);

        if (std::abs(result) < DeadZone)
            return 0.0f;

        return result;
    }

    bool Input::Initialize()
    {
        // TODO: Desktop inputs
        return true;
    }
    
    void Input::Shutdown()
    {
    }

    void Input::Update()
    {
        // TODO: Desktop inputs
    }

    const GamepadState& Input::GetState()
    {
        return _state;
    }

    bool Input::IsButtonHeld(GamepadButton button)
    {
        return (_state.buttons & ToMask(button)) != 0;
    }

    bool Input::IsButtonDown(GamepadButton button)
    {
        const std::uint32_t mask = ToMask(button);

        return (_state.buttons & mask) != 0 && (_state.previousButtons & mask) == 0;
    }

    bool Input::IsButtonUp(GamepadButton button)
    {
        const std::uint32_t mask = ToMask(button);

        return (_state.buttons & mask) == 0 && (_state.previousButtons & mask) != 0;
    }
}
