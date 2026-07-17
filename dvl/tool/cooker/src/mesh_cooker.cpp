#include "dvl/tool/cooker/mesh_cooker.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <fstream>
#include <vector>

#include "dvl/asset/mesh_format.h"
#include "dvl/log/log.h"

namespace dvl
{
    bool MeshCooker::Cook(const std::filesystem::path& source, const std::filesystem::path& destination) const
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
                                source.string(),
                                aiProcess_Triangulate |
                                aiProcess_JoinIdenticalVertices |
                                aiProcess_GenSmoothNormals |
                                aiProcess_ImproveCacheLocality);

        if (scene == nullptr || !scene->HasMeshes())
        {
            const std::string message = "Failed to import mesh '" + source.string() + "': " + importer.GetErrorString();
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        std::vector<MeshVertexFormat> vertices;
        std::vector<std::uint16_t> indices;

        for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
        {
            const aiMesh* mesh = scene->mMeshes[meshIndex];
            if (!mesh->HasPositions())
                continue;

            const std::size_t baseVertex = vertices.size();
            const std::size_t resultingVertexCount = baseVertex + mesh->mNumVertices;

            vertices.reserve(resultingVertexCount);
            for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
            {
                const aiVector3D& position = mesh->mVertices[vertexIndex];
                const aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[vertexIndex] : aiVector3D{};
                const aiVector3D uv = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][vertexIndex] : aiVector3D{};

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
