#include "dvl/tool/cooker/texture_cooker.h"

#include <fstream>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "dvl/asset/texture_format.h"
#include "dvl/log/log.h"

namespace dvl
{
    bool TextureCooker::Cook(const std::filesystem::path& source, const std::filesystem::path& destination) const
    {
        int width = 0;
        int height = 0;
        int channelCount = 0;

        stbi_uc* pixels = stbi_load(source.string().c_str(), &width, &height, &channelCount, STBI_rgb_alpha);

        if (!pixels || width <= 0 || height <= 0)
        {
            const char* failureReason = stbi_failure_reason();
            const std::string message = "Failed to load texture '" + source.string() + "': " +
                                        (failureReason != nullptr ? failureReason : "unknown error");
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        const TextureFileHeader header =
        {
            TextureMagic,
            TextureVersion,
            static_cast<std::uint32_t>(width),
            static_cast<std::uint32_t>(height)
        };

        const std::size_t dataSize = static_cast<std::size_t>(width) * height * STBI_rgb_alpha;

        std::ofstream output(destination, std::ios::binary);
        if (!output)
        {
            const std::string message = "Failed to open output file '" + destination.string() + "'";
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        output.write(reinterpret_cast<const char*>(&header), sizeof(header));
        output.write(reinterpret_cast<const char*>(pixels), static_cast<std::streamsize>(dataSize));

        if (!output)
        {
            const std::string message = "Failed to write output file '" + destination.string() + "'";
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        const std::string message = "Cooked " + source.string() + " -> " + destination.string();
        Log(LogLevel::Info, message.c_str());
        return true;
    }

    std::string TextureCooker::GetOutputExtension() const
    {
        return ".dvltex";
    }
}
