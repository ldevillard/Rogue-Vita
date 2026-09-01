#include "dvl/filesystem/filesystem.h"

namespace dvl
{
    std::string Filesystem::GetAssetPath(const std::string& path)
    {
        return "app0:/asset/" + path;
    }
}