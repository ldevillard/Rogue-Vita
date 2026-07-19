#pragma once

#include <cstdint>

namespace dvl
{
    class Time
    {
    public:
        static void Initialize();
        static void Update();

        static float GetDeltaTime();

    private:
        static std::uint64_t _previousTime;
        static float _deltaTime;
    };
}
