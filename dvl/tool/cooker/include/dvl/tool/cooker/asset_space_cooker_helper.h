#pragma once

#include <assimp/matrix4x4.h>
#include <assimp/scene.h>

namespace dvl
{
    class AssetSpaceCookerHelper
    {
    public:
        bool Initialize(const aiScene& scene);

        aiVector3D ToCookedPoint(const aiVector3D& point) const;
        aiVector3D ToCookedDirection(const aiVector3D& direction) const;
        aiMatrix4x4 ToSourceMatrix() const;

    private:
        aiVector3D _center;
        float _normalizationScale = 1.0f;
        int _upAxis = 1;
    };
}
