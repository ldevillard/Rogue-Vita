#pragma once

namespace dvl
{
    enum class LogLevel
    {
        Info,
        Warning,
        Error
    };

    void Log(LogLevel level, const char* message);
}
