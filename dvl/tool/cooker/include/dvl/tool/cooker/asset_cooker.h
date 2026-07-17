#pragma once

#include <filesystem>

namespace dvl
{
    class AssetCooker
    {
    public:
        virtual ~AssetCooker() = default;

        virtual bool Cook(const std::filesystem::path& source, const std::filesystem::path& destination) const = 0;

        virtual std::string GetOutputExtension() const = 0;
    };
}
