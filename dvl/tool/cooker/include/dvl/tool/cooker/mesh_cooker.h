#pragma once

#include "dvl/tool/cooker/asset_cooker.h"
#include "dvl/asset/mesh_format.h"

namespace dvl
{   
    class MeshCooker : public AssetCooker
    {
    public:
        bool Cook(const std::filesystem::path& source, const std::filesystem::path& destination) const override;

        std::string GetOutputExtension() const override;

    private:
        bool cook(const std::filesystem::path& source, const std::filesystem::path& destination, MeshType type) const;
    };
}
