#include "dvl/tool/cooker/mesh_cooker.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <fstream>
#include <limits>
#include <vector>

#include "dvl/asset/mesh_format.h"
#include "dvl/log/log.h"
#include "dvl/tool/cooker/asset_space_cooker_helper.h"

namespace dvl
{
    namespace
    {
        struct BoneInfluence
        {
            unsigned int index = 0;
            float weight = 0.0f;
        };

        void AddBoneInfluence(BoneInfluence* influences, unsigned int boneIndex, float weight)
        {
            if (weight <= 0.0f)
                return;

            unsigned int weakestIndex = 0;
            for (unsigned int index = 0; index < 4; index++)
            {
                BoneInfluence& influence = influences[index];
                if (influence.weight == 0.0f)
                {
                    influence = {boneIndex, weight};
                    return;
                }

                if (influence.weight < influences[weakestIndex].weight)
                    weakestIndex = index;
            }

            if (weight > influences[weakestIndex].weight)
            {
                influences[weakestIndex] = {boneIndex, weight};
            }
        }

        void EncodeBoneWeights(const BoneInfluence* influences, std::uint8_t* weights)
        {
            float totalWeight = 0.0f;
            for (unsigned int index = 0; index < 4; index++)
                totalWeight += influences[index].weight;

            if (totalWeight <= 0.0f)
                return;
            
            for (unsigned int index = 1; index < 4; index++)
                weights[index] = static_cast<std::uint8_t>(influences[index].weight / totalWeight * 255.0f);
            
            weights[0] = 255 - weights[1] - weights[2] - weights[3];
        }
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
                                aiProcess_FlipUVs);

        if (scene == nullptr || !scene->HasMeshes())
        {
            const std::string message = "Failed to import mesh '" + source.string() + "': " + importer.GetErrorString();
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        return cook(source, destination, scene->mMeshes[0]->HasBones() ? MeshType::Skinned : MeshType::Static);
    }

    bool MeshCooker::cook(const std::filesystem::path& source, const std::filesystem::path& destination, MeshType meshType) const
    {
        Assimp::Importer importer;
        unsigned int postProcess = aiProcess_Triangulate |
                                    aiProcess_JoinIdenticalVertices |
                                    aiProcess_GenSmoothNormals |
                                    aiProcess_ImproveCacheLocality |
                                    aiProcess_FlipUVs;

        const aiScene* scene = importer.ReadFile(source.string(), postProcess);

        if (scene == nullptr || !scene->HasMeshes())
        {
            const std::string message = "Failed to import mesh '" + source.string() + "': " + importer.GetErrorString();
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        AssetSpaceCookerHelper assetSpace;
        if (!assetSpace.Initialize(*scene))
        {
            const std::string message = "Failed to determine mesh space for '" + source.string() + "'";
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        std::vector<MeshVertexFormat> vertices;
        std::vector<std::uint16_t> indices;
        for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
        {
            const aiMesh* mesh = scene->mMeshes[meshIndex];
            if (!mesh->HasPositions())
                continue;

            if (meshType == MeshType::Skinned && mesh->mNumBones > std::numeric_limits<std::uint8_t>::max() + 1u)
            {
                const std::string message = "Skinned mesh '" + source.string() + "' has more than 256 bones";
                Log(LogLevel::Error, message.c_str());
                return false;
            }

            const std::size_t baseVertex = vertices.size();
            const std::size_t resultingVertexCount = baseVertex + mesh->mNumVertices;
            if (resultingVertexCount > std::numeric_limits<std::uint16_t>::max())
            {
                const std::string message = "Mesh '" + source.string() + "' exceeds the 65,535 vertex limit";
                Log(LogLevel::Error, message.c_str());
                return false;
            }

            std::vector<BoneInfluence> meshInfluences(mesh->mNumVertices * 4);
            for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
            {
                const aiBone* bone = mesh->mBones[boneIndex];
                for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++)
                {
                    const aiVertexWeight& weight = bone->mWeights[weightIndex];
                    AddBoneInfluence(&meshInfluences[weight.mVertexId * 4], boneIndex, weight.mWeight);
                }
            }

            vertices.reserve(resultingVertexCount);
            for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
            {
                const aiVector3D position = assetSpace.ToCookedPoint(mesh->mVertices[vertexIndex]);
                const aiVector3D sourceNormal = mesh->HasNormals() ? mesh->mNormals[vertexIndex] : aiVector3D{};
                const aiVector3D uv = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][vertexIndex] : aiVector3D{};
                const aiVector3D normal = assetSpace.ToCookedDirection(sourceNormal);
                MeshVertexFormat vertex =
                {
                    position.x, position.y, position.z,
                    normal.x, normal.y, normal.z,
                    uv.x, uv.y,
                    {}, {}
                };

                if (meshType == MeshType::Skinned)
                {
                    BoneInfluence* influences = &meshInfluences[vertexIndex * 4];

                    for (unsigned int influenceIndex = 0; influenceIndex < 4; influenceIndex++)
                        vertex.boneIndices[influenceIndex] = static_cast<std::uint8_t>(influences[influenceIndex].index);
                    
                    EncodeBoneWeights(influences, vertex.boneWeights);
                }

                vertices.push_back(vertex);
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
            static_cast<std::uint32_t>(indices.size()),
            meshType
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
