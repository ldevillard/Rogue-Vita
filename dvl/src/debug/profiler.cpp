#include "dvl/debug/profiler.h"

#include <string>

#include "dvl/log/log.h"
#include "dvl/time/time.h"

namespace dvl
{
    Profiler::Profiler(const char* name)
        : _name(name), _startTime(Time::GetCurrentTimeMicroseconds())
    {
    }

    Profiler::~Profiler()
    {
        const float milliseconds = static_cast<float>(Time::GetCurrentTimeMicroseconds() - _startTime) / 1000.0f;

        const std::string message = "[Profiler] " + std::string(_name) + ": " + std::to_string(milliseconds) + " ms";
        Log(LogLevel::Info, message.c_str());
    }
}
