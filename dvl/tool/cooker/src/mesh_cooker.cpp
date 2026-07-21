#include "dvl/tool/cooker/mesh_cooker.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <algorithm>
#include <fstream>
#include <limits>
#include <vector>

#include "dvl/asset/mesh_format.h"
#include "dvl/log/log.h"

namespace dvl
{
    aiVector3D ConvertZUpToYUp(const aiVector3D& value)
    {
        return {value.x, value.z, -value.y};
    }

    bool MeshCooker::Cook(const std::filesystem::path& source, const std::filesystem::path& destination) const
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
                                source.string(),
                                aiProcess_Triangulate |
                                aiProcess_JoinIdenticalVertices |
                                aiProcess_GenSmoothNormals |
                                aiProcess_ImproveCacheLocality |
                                aiProcess_PreTransformVertices |
                                aiProcess_FlipUVs);

        if (scene == nullptr || !scene->HasMeshes())
        {
            const std::string message = "Failed to import mesh '" + source.string() + "': " + importer.GetErrorString();
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        int upAxis = 1;
        if (scene->mMetaData != nullptr)
            scene->mMetaData->Get("UpAxis", upAxis);

        if (upAxis != 1 && upAxis != 2)
        {
            const std::string message = "Unsupported up axis in mesh '" + source.string() + "'";
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        const bool requiresYUpConversion = upAxis == 2;

        std::vector<MeshVertexFormat> vertices;
        std::vector<std::uint16_t> indices;

        aiVector3D minBounds
        {
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
        };
        
        aiVector3D maxBounds
        {
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest()
        };

        for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
        {
            const aiMesh* mesh = scene->mMeshes[meshIndex];
            if (!mesh->HasPositions())
                continue;

            for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
            {
                const aiVector3D& position = mesh->mVertices[vertexIndex];
                minBounds.x = std::min(minBounds.x, position.x);
                minBounds.y = std::min(minBounds.y, position.y);
                minBounds.z = std::min(minBounds.z, position.z);
                maxBounds.x = std::max(maxBounds.x, position.x);
                maxBounds.y = std::max(maxBounds.y, position.y);
                maxBounds.z = std::max(maxBounds.z, position.z);
            }
        }

        const aiVector3D center = (minBounds + maxBounds) * 0.5f;
        const aiVector3D size = maxBounds - minBounds;
        const float largestDimension = std::max({size.x, size.y, size.z});
        const float normalizationScale = largestDimension > 0.0f ? 1.0f / largestDimension : 1.0f;

        for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
        {
            const aiMesh* mesh = scene->mMeshes[meshIndex];
            if (!mesh->HasPositions())
                continue;

            const std::size_t baseVertex = vertices.size();
            const std::size_t resultingVertexCount = baseVertex + mesh->mNumVertices;

            if (resultingVertexCount > std::numeric_limits<std::uint16_t>::max())
            {
                const std::string message = "Mesh '" + source.string() + "' exceeds the 65,535 vertex limit";
                Log(LogLevel::Error, message.c_str());
                return false;
            }

            vertices.reserve(resultingVertexCount);
            for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
            {
                const aiVector3D sourcePosition = (mesh->mVertices[vertexIndex] - center) * normalizationScale;
                const aiVector3D sourceNormal = mesh->HasNormals() ? mesh->mNormals[vertexIndex] : aiVector3D{};
                const aiVector3D uv = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][vertexIndex] : aiVector3D{};

                const aiVector3D position = requiresYUpConversion ? ConvertZUpToYUp(sourcePosition) : sourcePosition;
                const aiVector3D normal = requiresYUpConversion ? ConvertZUpToYUp(sourceNormal) : sourceNormal;

                vertices.push_back
                ({
                    position.x, position.y, position.z,
                    normal.x, normal.y, normal.z,
                    uv.x, uv.y
                });
            }

            for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
            {
                const aiFace& face = mesh->mFaces[faceIndex];
                if (face.mNumIndices != 3)
                    continue;

                indices.push_back(static_cast<std::uint16_t>(baseVertex + face.mIndices[0]));
                indices.push_back(static_cast<std::uint16_t>(baseVertex + face.mIndices[1]));
                indices.push_back(static_cast<std::uint16_t>(baseVertex + face.mIndices[2]));
            }
        }

        if (vertices.empty() || indices.empty())
        {
            const std::string message = "No triangle geometry found in '" + source.string() + "'";
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        const MeshFileHeader header = 
        {
            MeshMagic,
            MeshVersion,
            static_cast<std::uint32_t>(vertices.size()),
            static_cast<std::uint32_t>(indices.size())
        };

        std::ofstream output(destination, std::ios::binary);
        if (!output)
        {
            const std::string message = "Failed to open output file '" + destination.string() + "'";
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        output.write(reinterpret_cast<const char*>(&header), sizeof(header));
        output.write(reinterpret_cast<const char*>(vertices.data()), static_cast<std::streamsize>(vertices.size() * sizeof(MeshVertexFormat)));
        output.write(reinterpret_cast<const char*>(indices.data()), static_cast<std::streamsize>(indices.size() * sizeof(std::uint16_t)));

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

    std::string MeshCooker::GetOutputExtension() const
    {
        return ".dvlmesh";
    }
}
