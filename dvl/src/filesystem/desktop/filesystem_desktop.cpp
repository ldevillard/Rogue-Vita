#include "dvl/filesystem/filesystem.h"

namespace dvl
{
    std::string Filesystem::GetAssetPath(const std::string& path)
    {
        return "asset/" + path;
    }
}