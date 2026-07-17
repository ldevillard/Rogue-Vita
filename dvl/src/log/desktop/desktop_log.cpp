#include "dvl/log/log.h"

#include <cstdio>

namespace dvl
{
    void Log(LogLevel level, const char* message)
    {
        const char* prefix = "[INFO]";
        std::FILE* output = stdout;

        switch (level)
        {
            case LogLevel::Info:
                break;

            case LogLevel::Warning:
                prefix = "[WARNING]";
                break;

            case LogLevel::Error:
                prefix = "[ERROR]";
                output = stderr;
                break;
        }

        std::fprintf(output, "[DVL] %s %s\n", prefix, message);
    }
}
