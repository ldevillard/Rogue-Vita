#pragma once

#include <cstdint>

#ifndef DVL_PROFILING_ENABLED
#define DVL_PROFILING_ENABLED 1
#endif

namespace dvl
{
    class Profiler
    {
    public:
        Profiler(const char* name);
        ~Profiler();

    private:
        const char* _name;
        std::uint64_t _startTime;
    };
}

#if DVL_PROFILING_ENABLED
#define DVL_PROFILE_SCOPE(name) dvl::Profiler profiler(name)
#define DVL_PROFILE_FUNCTION() DVL_PROFILE_SCOPE(__func__)
#else
#define DVL_PROFILE_SCOPE(name)
#define DVL_PROFILE_FUNCTION()
#endif
