#include "dvl/input/input.h"

#include <GLFW/glfw3.h>
#include <cmath>

namespace dvl
{
    GamepadState Input::_state {};

    namespace
    {
        bool IsKeyPressed(GLFWwindow* window, int key)
        {
            return glfwGetKey(window, key) == GLFW_PRESS;
        }

        float GetKeyboardAxis(GLFWwindow* window, int negativeKey, int positiveKey)
        {
            const float negative = IsKeyPressed(window, negativeKey) ? 1.0f : 0.0f;
            const float positive = IsKeyPressed(window, positiveKey) ? 1.0f : 0.0f;

            return positive - negative;
        }

        void UpdateKeyboardStick(GamepadState& state)
        {
            GLFWwindow* window = glfwGetCurrentContext();
            if (window == nullptr)
                return;

            const bool horizontalInput = IsKeyPressed(window, GLFW_KEY_A) || IsKeyPressed(window, GLFW_KEY_D);
            const bool verticalInput = IsKeyPressed(window, GLFW_KEY_S) || IsKeyPressed(window, GLFW_KEY_W);

            if (horizontalInput)
                state.leftStick.x = GetKeyboardAxis(window, GLFW_KEY_A, GLFW_KEY_D);

            if (verticalInput)
                state.leftStick.y = GetKeyboardAxis(window, GLFW_KEY_S, GLFW_KEY_W);
        }
    }

    std::uint32_t ToMask(GamepadButton button)
    {
        return static_cast<std::uint32_t>(button);
    }

    float NormalizeStick(float value)
    {
        constexpr float DeadZone = 0.15f;

        if (std::abs(value) < DeadZone)
            return 0.0f;

        return value;
    }

    bool Input::Initialize()
    {
        return true;
    }
    
    void Input::Shutdown()
    {
    }

    void Input::Update()
    {
        _state.previousButtons = _state.buttons;
        _state.buttons = 0;

        _state.leftStick = {};
        _state.rightStick = {};

        GLFWgamepadstate state {};

        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state) == GLFW_TRUE)
        {
            _state.leftStick.x = NormalizeStick(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
            _state.leftStick.y = -NormalizeStick(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);

            _state.rightStick.x = NormalizeStick(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);
            _state.rightStick.y = -NormalizeStick(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_A])
                _state.buttons |= ToMask(GamepadButton::Cross);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_B])
                _state.buttons |= ToMask(GamepadButton::Circle);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_X])
                _state.buttons |= ToMask(GamepadButton::Square);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_Y])
                _state.buttons |= ToMask(GamepadButton::Triangle);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP])
                _state.buttons |= ToMask(GamepadButton::Up);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN])
                _state.buttons |= ToMask(GamepadButton::Down);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT])
                _state.buttons |= ToMask(GamepadButton::Left);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT])
                _state.buttons |= ToMask(GamepadButton::Right);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER])
                _state.buttons |= ToMask(GamepadButton::L);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER])
                _state.buttons |= ToMask(GamepadButton::R);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_START])
                _state.buttons |= ToMask(GamepadButton::Start);

            if (state.buttons[GLFW_GAMEPAD_BUTTON_BACK])
                _state.buttons |= ToMask(GamepadButton::Select);
        }

        UpdateKeyboardStick(_state);
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
