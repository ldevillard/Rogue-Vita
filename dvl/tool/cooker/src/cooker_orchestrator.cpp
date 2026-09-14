#include "dvl/tool/cooker/cooker_orchestrator.h"

#include "dvl/log/log.h"
#include "dvl/tool/cooker/animation_cooker.h"
#include "dvl/tool/cooker/mesh_cooker.h"
#include "dvl/tool/cooker/texture_cooker.h"
#include "dvl/tool/cooker/skeleton_cooker.h"

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

        const bool meshSuccess = cookMesh();
        const bool textureSuccess = cookTexture();
        const bool skeletonSuccess = cookSkeleton();
        const bool animationSuccess = cookAnimation();
        return meshSuccess && textureSuccess && skeletonSuccess && animationSuccess;
    }

    bool CookerOrchestrator::cookMesh() const
    {
        MeshCooker meshCooker;
        bool success = true;

        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(_sourcePath / "mesh"))
        {
            const std::filesystem::path& source = entry.path();
            const std::filesystem::path extension = source.extension();

            if (!entry.is_regular_file() || (extension != ".fbx" && extension != ".FBX"))
                continue;

            std::filesystem::path destination = _destinationPath / "mesh" / source.filename();
            destination.replace_extension(meshCooker.GetOutputExtension());

            if (!meshCooker.Cook(source, destination))
                success = false;
        }

        return success;
    }

    bool CookerOrchestrator::cookTexture() const
    {
        TextureCooker textureCooker;
        bool success = true;

        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(_sourcePath / "texture"))
        {
            const std::filesystem::path& source = entry.path();
            const std::filesystem::path extension = source.extension();

            if (!entry.is_regular_file() || (extension != ".png" && extension != ".PNG"))
                continue;

            std::filesystem::path destination = _destinationPath / "texture" / source.stem();
            destination.replace_extension(textureCooker.GetOutputExtension());

            if (!textureCooker.Cook(source, destination))
                success = false;
        }

        return success;
    }

    bool CookerOrchestrator::cookSkeleton() const
    {
        SkeletonCooker skeletonCooker;
        bool success = true;

        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(_sourcePath / "mesh"))
        {
            const std::filesystem::path& source = entry.path();
            const std::filesystem::path extension = source.extension();

            if (!entry.is_regular_file() || (extension != ".fbx" && extension != ".FBX"))
                continue;

            std::filesystem::path destination = _destinationPath / "skeleton" / source.stem();
            destination.replace_extension(skeletonCooker.GetOutputExtension());

            if (!skeletonCooker.Cook(source, destination))
                success = false;
        }

        return success;
    }

    bool CookerOrchestrator::cookAnimation() const
    {
        AnimationCooker animationCooker;
        bool success = true;

        const std::filesystem::path sourceDirectory = _sourcePath / "animation";
        if (!std::filesystem::exists(sourceDirectory))
            return true;

        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(sourceDirectory))
        {
            const std::filesystem::path& source = entry.path();
            const std::filesystem::path extension = source.extension();

            if (!entry.is_regular_file() || (extension != ".fbx" && extension != ".FBX"))
                continue;

            std::filesystem::path destination = _destinationPath / "animation" / source.stem();
            destination.replace_extension(animationCooker.GetOutputExtension());

            if (!animationCooker.Cook(source, destination))
                success = false;
        }

        return success;
    }
}
