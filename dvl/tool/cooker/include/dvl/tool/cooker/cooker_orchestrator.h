#pragma once

#include <filesystem>

namespace dvl
{
    class CookerOrchestrator
    {
    public:
        CookerOrchestrator(std::filesystem::path sourcePath, std::filesystem::path destinationPath);

        bool Cook();

    private:
        bool cookMesh() const;

        std::filesystem::path _sourcePath;
        std::filesystem::path _destinationPath;
    };
}
