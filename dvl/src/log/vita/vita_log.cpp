#include <dvl/log/log.h>

#include <psp2/kernel/clib.h>

namespace dvl
{
    void Log(LogLevel level, const char* message)
    {
        const char* prefix = "[INFO]";

        switch (level)
        {
            case LogLevel::Info:
                prefix = "[INFO]";
                break;

            case LogLevel::Warning:
                prefix = "[WARNING]";
                break;

            case LogLevel::Error:
                prefix = "[ERROR]";
                break;
        }

        sceClibPrintf("[DVL] %s %s\n", prefix, message);
    }
}
