#pragma once

#include <assimp/scene.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace dvl
{
    // Same parent-first order for meshes, skeletons and animations
    class BoneOrderCookerHelper
    {
    public:
        bool Initialize(const aiScene& scene, const aiMesh& mesh);

        std::vector<const aiBone*> bones;
        std::unordered_map<std::string, unsigned int> indices;
    };
}
