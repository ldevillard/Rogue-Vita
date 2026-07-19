#include "dvl/time/time.h"

#include <psp2/kernel/processmgr.h>

#include <algorithm>

namespace dvl
{
    std::uint64_t Time::_previousTime = 0;
    float Time::_deltaTime = 0.0f;

    void Time::Initialize()
    {
        _previousTime = sceKernelGetProcessTimeWide();
        _deltaTime = 0.0f;
    }

    void Time::Update()
    {
        constexpr float MicrosecondsToSeconds = 1.0f / 1'000'000.0f;
        constexpr float MaximumDeltaTime = 0.1f;

        const std::uint64_t currentTime = sceKernelGetProcessTimeWide();
        const std::uint64_t elapsedTime = currentTime - _previousTime;

        _previousTime = currentTime;
        _deltaTime = std::min(static_cast<float>(elapsedTime) * MicrosecondsToSeconds, MaximumDeltaTime);
    }

    float Time::GetDeltaTime()
    {
        return _deltaTime;
    }
}
