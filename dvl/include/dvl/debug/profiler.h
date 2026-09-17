#pragma once

#include <cstdint>

#ifndef DVL_PROFILING_ENABLED
#define DVL_PROFILING_ENABLED 1
#endif

#define DVL_PROFILING_LOG_INTERVAL 30

namespace dvl
{
    class Profiler
    {
    public:
        Profiler(const char* name, bool shouldLog);
        ~Profiler();

    private:
        const char* _name;
        std::uint64_t _startTime;
        bool _shouldLog;
    };
}

#if DVL_PROFILING_ENABLED
#define DVL_PROFILE_SCOPE(name) \
    static std::uint32_t profilerCount = 0; \
    dvl::Profiler profiler(name, ++profilerCount % DVL_PROFILING_LOG_INTERVAL == 0)
#define DVL_PROFILE_FUNCTION() DVL_PROFILE_SCOPE(__func__)
#define DVL_PROFILE_FUNCTION_PRETTY() DVL_PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
#define DVL_PROFILE_SCOPE(name)
#define DVL_PROFILE_FUNCTION()
#define DVL_PROFILE_FUNCTION_PRETTY()
#endif
