#include "dvl/time/time.h"

#include <algorithm>
#include <chrono>

namespace dvl
{
    std::uint64_t Time::_previousTime = 0;
    float Time::_deltaTime = 0.0f;

    static std::uint64_t GetCurrentTime()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    void Time::Initialize()
    {
        _previousTime = GetCurrentTime();
        _deltaTime = 0.0f;
    }

    void Time::Update()
    {
        constexpr float MicrosecondsToSeconds = 1.0f / 1'000'000.0f;
        constexpr float MaximumDeltaTime = 0.1f;

        const std::uint64_t currentTime = GetCurrentTime();
        const std::uint64_t elapsedTime = currentTime - _previousTime;

        _previousTime = currentTime;
        _deltaTime = std::min(elapsedTime * MicrosecondsToSeconds, MaximumDeltaTime);
    }

    float Time::GetDeltaTime()
    {
        return _deltaTime;
    }
}