#include "dvl/tool/cooker/asset_space_cooker_helper.h"

#include <algorithm>
#include <limits>

namespace dvl
{
    bool AssetSpaceCookerHelper::Initialize(const aiScene& scene)
    {
        if (scene.mMetaData != nullptr)
            scene.mMetaData->Get("UpAxis", _upAxis);

        if (_upAxis != 1 && _upAxis != 2)
            return false;

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

        bool hasPositions = false;

        for (unsigned int meshIndex = 0; meshIndex < scene.mNumMeshes; meshIndex++)
        {
            const aiMesh* mesh = scene.mMeshes[meshIndex];
            if (!mesh->HasPositions())
                continue;

            hasPositions = true;

            for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++)
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

        if (!hasPositions)
            return false;

        _center = (minBounds + maxBounds) * 0.5f;

        const aiVector3D size = maxBounds - minBounds;
        
        const float largestDimension = std::max({size.x, size.y, size.z});
        
        _normalizationScale = largestDimension > 0.0f ? 1.0f / largestDimension : 1.0f;
        
        return true;
    }

    aiVector3D AssetSpaceCookerHelper::ToCookedPoint(const aiVector3D& point) const
    {
        return ToCookedDirection((point - _center) * _normalizationScale);
    }

    aiVector3D AssetSpaceCookerHelper::ToCookedDirection(const aiVector3D& direction) const
    {
        return _upAxis == 2 ? aiVector3D{-direction.x, direction.z, direction.y} : direction;
    }

    aiMatrix4x4 AssetSpaceCookerHelper::ToSourceMatrix() const
    {
        const float scale = 1.0f / _normalizationScale;

        return _upAxis == 2
            ? aiMatrix4x4(-scale, 0.0f, 0.0f, _center.x,
                          0.0f, 0.0f, scale, _center.y,
                          0.0f, scale, 0.0f, _center.z,
                          0.0f, 0.0f, 0.0f, 1.0f)
            : aiMatrix4x4(scale, 0.0f, 0.0f, _center.x,
                          0.0f, scale, 0.0f, _center.y,
                          0.0f, 0.0f, scale, _center.z,
                          0.0f, 0.0f, 0.0f, 1.0f);
    }
}
