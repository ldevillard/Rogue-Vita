#pragma once

namespace dvl
{
    enum class LogLevel
    {
        Info,
        Warning,
        Error
    };

    // TODO: Refacto to support variadic args, like printf
    void Log(LogLevel level, const char* message);
}
