#include "dvl/input/input.h"

#include <psp2/ctrl.h>

#include <algorithm>

namespace dvl
{
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
        return sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG) >= 0;
    }
    
    void Input::Shutdown()
    {
    }

    void Input::Update()
    {
        _state.previousButtons = _state.buttons;
        _state.buttons = 0;

        SceCtrlData data {};

        if (sceCtrlPeekBufferPositive(0, &data, 1) <= 0)
            return;

        _state.leftStick.x = NormalizeStick(data.lx);
        _state.leftStick.y = -NormalizeStick(data.ly);

        _state.rightStick.x = NormalizeStick(data.rx);
        _state.rightStick.y = -NormalizeStick(data.ry);

        if (data.buttons & SCE_CTRL_CROSS)
            _state.buttons |= ToMask(GamepadButton::Cross);
        
        if (data.buttons & SCE_CTRL_CIRCLE)
            _state.buttons |= ToMask(GamepadButton::Circle);

        if (data.buttons & SCE_CTRL_SQUARE)
            _state.buttons |= ToMask(GamepadButton::Square);

        if (data.buttons & SCE_CTRL_TRIANGLE)
            _state.buttons |= ToMask(GamepadButton::Triangle);

        if (data.buttons & SCE_CTRL_UP)
            _state.buttons |= ToMask(GamepadButton::Up);

        if (data.buttons & SCE_CTRL_DOWN)
            _state.buttons |= ToMask(GamepadButton::Down);

        if (data.buttons & SCE_CTRL_LEFT)
            _state.buttons |= ToMask(GamepadButton::Left);

        if (data.buttons & SCE_CTRL_RIGHT)
            _state.buttons |= ToMask(GamepadButton::Right);

        if (data.buttons & SCE_CTRL_LTRIGGER)
            _state.buttons |= ToMask(GamepadButton::L);

        if (data.buttons & SCE_CTRL_RTRIGGER)
            _state.buttons |= ToMask(GamepadButton::R);

        if (data.buttons & SCE_CTRL_START)
            _state.buttons |= ToMask(GamepadButton::Start);

        if (data.buttons & SCE_CTRL_SELECT)
            _state.buttons |= ToMask(GamepadButton::Select);
    }

    const GamepadState& Input::GetState() const
    {
        return _state;
    }

    bool Input::IsButtonHeld(GamepadButton button) const
    {
        return (_state.buttons & ToMask(button)) != 0;
    }

    bool Input::IsButtonDown(GamepadButton button) const
    {
        const std::uint32_t mask = ToMask(button);

        return (_state.buttons & mask) != 0 && (_state.previousButtons & mask) == 0;
    }

    bool Input::IsButtonUp(GamepadButton button) const
    {
        const std::uint32_t mask = ToMask(button);

        return (_state.buttons & mask) == 0 && (_state.previousButtons & mask) != 0;
    }
}