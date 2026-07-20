#pragma once

#include "dvl/tool/cooker/asset_cooker.h"

namespace dvl
{
    class TextureCooker : public AssetCooker
    {
    public:
        bool Cook(const std::filesystem::path& source, const std::filesystem::path& destination) const override;

        std::string GetOutputExtension() const override;
    };
}
