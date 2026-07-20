#include <filesystem>

#include "dvl/log/log.h"
#include "dvl/tool/cooker/cooker_orchestrator.h"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        dvl::Log(dvl::LogLevel::Error, "Usage: dvl-cooker <input path> <output path>");
        return 1;
    }

    dvl::CookerOrchestrator cookerOrchestrator {std::filesystem::path(argv[1]),
                                                std::filesystem::path(argv[2])};

    return cookerOrchestrator.Cook() ? 0 : 1;
}
