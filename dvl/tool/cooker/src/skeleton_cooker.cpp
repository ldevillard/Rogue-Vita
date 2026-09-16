#include "dvl/tool/cooker/skeleton_cooker.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <fstream>
#include <vector>

#include "dvl/log/log.h"
#include "dvl/math/math.h"
#include "dvl/tool/cooker/asset_space_cooker_helper.h"
#include "dvl/tool/cooker/bone_order_cooker_helper.h"

#include "dvl/asset/skeleton_format.h"

namespace dvl
{
    static Mat4 ToMat4(const aiMatrix4x4& matrix)
    {
        Mat4 result;

        for (unsigned int column = 0; column < 4; column++)
        {
            for (unsigned int row = 0; row < 4; row++)
                result[column][row] = matrix[row][column];
        }

        return result;
    }

    bool SkeletonCooker::Cook(const std::filesystem::path& source, const std::filesystem::path& destination) const
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(source.string(), 0);

        if (scene == nullptr || !scene->HasMeshes())
        {
            const std::string message = "Failed to import skeleton '" + source.string() + "': " + importer.GetErrorString();
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        const aiMesh* mesh = scene->mMeshes[0];
        if (!mesh->HasBones())
        {
            // Static meshes do not produce a skeleton
            return true;
        }

        AssetSpaceCookerHelper assetSpace;
        if (!assetSpace.Initialize(*scene))
        {
            const std::string message = "Failed to determine mesh space for skeleton '" + source.string() + "'";
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        const aiMatrix4x4 bindTransform = assetSpace.ToSourceMatrix();

        std::vector<std::int16_t> parents(mesh->mNumBones, -1);
        std::vector<Mat4> inverseBindMatrices(mesh->mNumBones);
        
        BoneOrderCookerHelper boneOrder;
        if (!boneOrder.Initialize(*scene, *mesh))
            return false;

        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
        {
            const aiBone* bone = boneOrder.bones[boneIndex];

            inverseBindMatrices[boneIndex] = ToMat4(bone->mOffsetMatrix * bindTransform);

            const aiNode* boneNode = scene->mRootNode->FindNode(bone->mName);
            if (boneNode->mParent == nullptr)
                continue;

            const auto parent = boneOrder.indices.find(boneNode->mParent->mName.C_Str());
            if (parent != boneOrder.indices.end())
                parents[boneIndex] = static_cast<std::int16_t>(parent->second);
        }

        const SkeletonFileHeader header =
        {
            SkeletonMagic,
            SkeletonVersion,
            mesh->mNumBones
        };

        std::ofstream output(destination, std::ios::binary);
        if (!output)
        {
            const std::string message = "Failed to open output file '" + destination.string() + "'";
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        output.write(reinterpret_cast<const char*>(&header), sizeof(header));
        output.write(reinterpret_cast<const char*>(parents.data()), static_cast<std::streamsize>(parents.size() * sizeof(std::int16_t)));
        output.write(reinterpret_cast<const char*>(inverseBindMatrices.data()), static_cast<std::streamsize>(inverseBindMatrices.size() * sizeof(Mat4)));

        const std::string message = "Cooked " + source.string() + " -> " + destination.string();
        Log(LogLevel::Info, message.c_str());

        return true;
    }

    std::string SkeletonCooker::GetOutputExtension() const
    {
        return ".dvlskel";
    }
}
