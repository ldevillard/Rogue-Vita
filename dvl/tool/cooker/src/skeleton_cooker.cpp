#include "dvl/tool/cooker/skeleton_cooker.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <vector>

#include "dvl/log/log.h"
#include "dvl/math/math.h"

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
            const std::string message = "No bones found in skeleton '" + source.string() + "'";
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        Assimp::Importer meshImporter;
        const aiScene* meshScene = meshImporter.ReadFile(
            source.string(),
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_GenSmoothNormals |
            aiProcess_ImproveCacheLocality |
            aiProcess_PreTransformVertices |
            aiProcess_FlipUVs |
            aiProcess_GenBoundingBoxes);

        if (meshScene == nullptr || !meshScene->HasMeshes())
        {
            const std::string message = "Failed to import mesh space for skeleton '" + source.string() + "': " + meshImporter.GetErrorString();
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        aiAABB bounds = meshScene->mMeshes[0]->mAABB;
        for (unsigned int meshIndex = 1; meshIndex < meshScene->mNumMeshes; meshIndex++)
        {
            const aiAABB& meshBounds = meshScene->mMeshes[meshIndex]->mAABB;
            bounds.mMin.x = std::min(bounds.mMin.x, meshBounds.mMin.x);
            bounds.mMin.y = std::min(bounds.mMin.y, meshBounds.mMin.y);
            bounds.mMin.z = std::min(bounds.mMin.z, meshBounds.mMin.z);
            bounds.mMax.x = std::max(bounds.mMax.x, meshBounds.mMax.x);
            bounds.mMax.y = std::max(bounds.mMax.y, meshBounds.mMax.y);
            bounds.mMax.z = std::max(bounds.mMax.z, meshBounds.mMax.z);
        }

        const aiVector3D center = (bounds.mMin + bounds.mMax) * 0.5f;
        const aiVector3D size = bounds.mMax - bounds.mMin;
        const float largestDimension = std::max({size.x, size.y, size.z});
        const float assetScale = largestDimension > 0.0f ? largestDimension : 1.0f;

        int upAxis = 1;
        if (meshScene->mMetaData != nullptr)
            meshScene->mMetaData->Get("UpAxis", upAxis);

        if (upAxis != 1 && upAxis != 2)
        {
            const std::string message = "Unsupported up axis in skeleton '" + source.string() + "'";
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        const aiMatrix4x4 assetToMeshSpace = upAxis == 2
            ? aiMatrix4x4(-assetScale, 0.0f, 0.0f, center.x,
                            0.0f, 0.0f, assetScale, center.y,
                            0.0f, assetScale, 0.0f, center.z,
                            0.0f, 0.0f, 0.0f, 1.0f)
            : aiMatrix4x4(assetScale, 0.0f, 0.0f, center.x,
                          0.0f, assetScale, 0.0f, center.y,
                          0.0f, 0.0f, assetScale, center.z,
                          0.0f, 0.0f, 0.0f, 1.0f);

        const aiNode* meshNode = scene->mRootNode->FindNode(mesh->mName);
        if (meshNode == nullptr)
        {
            const std::string message = "Failed to find mesh node for skeleton '" + source.string() + "'";
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        aiMatrix4x4 meshTransform;
        for (const aiNode* node = meshNode; node != nullptr; node = node->mParent)
            meshTransform = node->mTransformation * meshTransform;

        meshTransform.Inverse();
        const aiMatrix4x4 bindTransform = meshTransform * assetToMeshSpace;

        std::vector<std::int16_t> parents(mesh->mNumBones, -1);
        std::vector<Mat4> inverseBindMatrices(mesh->mNumBones);
        std::unordered_map<std::string, std::int16_t> boneIndices;
        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
            boneIndices.emplace(mesh->mBones[boneIndex]->mName.C_Str(), static_cast<std::int16_t>(boneIndex));

        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
        {
            const aiBone* bone = mesh->mBones[boneIndex];

            inverseBindMatrices[boneIndex] = ToMat4(bone->mOffsetMatrix * bindTransform);

            const aiNode* boneNode = scene->mRootNode->FindNode(bone->mName);
            if (boneNode == nullptr || boneNode->mParent == nullptr)
                continue;

            const auto parent = boneIndices.find(boneNode->mParent->mName.C_Str());
            if (parent != boneIndices.end())
                parents[boneIndex] = parent->second;
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
