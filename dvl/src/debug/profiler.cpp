#include "dvl/debug/profiler.h"

#include <string>

#include "dvl/log/log.h"
#include "dvl/time/time.h"

namespace dvl
{
    Profiler::Profiler(const char* name, bool shouldLog)
        : _name(name), _startTime(shouldLog ? Time::GetCurrentTimeMicroseconds() : 0), _shouldLog(shouldLog)
    {
    }

    Profiler::~Profiler()
    {
        if (!_shouldLog)
            return;

        const float milliseconds = static_cast<float>(Time::GetCurrentTimeMicroseconds() - _startTime) / 1000.0f;

        const std::string message = "[Profiler] " + std::string(_name) + ": " + std::to_string(milliseconds) + " ms";
        Log(LogLevel::Info, message.c_str());
    }
}
