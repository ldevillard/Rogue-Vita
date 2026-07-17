#include "dvl/tool/cooker/cooker_orchestrator.h"

#include "dvl/log/log.h"
#include "dvl/tool/cooker/mesh_cooker.h"

namespace dvl
{
    CookerOrchestrator::CookerOrchestrator(std::filesystem::path sourcePath, std::filesystem::path destinationPath)
        : _sourcePath(std::move(sourcePath)),
          _destinationPath(std::move(destinationPath))
    {
    }

    bool CookerOrchestrator::Cook()
    {
        if (!std::filesystem::exists(_sourcePath) || !std::filesystem::is_directory(_sourcePath))
        {
            const std::string message = "Source path is not a directory: " + _sourcePath.string();
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        if (!std::filesystem::exists(_destinationPath) || !std::filesystem::is_directory(_destinationPath))
        {
            const std::string message = "Destination path is not a directory: " + _destinationPath.string();
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        return cookMesh();
    }

    bool CookerOrchestrator::cookMesh() const
    {
        MeshCooker meshCooker;
        bool success = true;

        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(_sourcePath))
        {
            const std::filesystem::path& source = entry.path();
            const std::filesystem::path extension = source.extension();

            if (!entry.is_regular_file() || (extension != ".fbx" && extension != ".FBX"))
                continue;

            std::filesystem::path destination = _destinationPath / source.filename();
            destination.replace_extension(meshCooker.GetOutputExtension());

            if (!meshCooker.Cook(source, destination))
                success = false;
        }

        return success;
    }
}
