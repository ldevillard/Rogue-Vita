#pragma once

#include <string>

namespace dvl
{
    class Filesystem
    {
    public:
        static std::string GetAssetPath(const std::string& path);
    };
}