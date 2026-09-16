#include "dvl/tool/cooker/bone_order_cooker_helper.h"

#include <algorithm>
#include <utility>

#include "dvl/log/log.h"

namespace dvl
{
    bool BoneOrderCookerHelper::Initialize(const aiScene& scene, const aiMesh& mesh)
    {
        bones.clear();
        indices.clear();

        if (scene.mRootNode == nullptr || mesh.mNumBones > 256)
        {
            Log(LogLevel::Error, "Invalid skeleton or more than 256 bones");
            return false;
        }

        for (unsigned int i = 0; i < mesh.mNumBones; i++)
        {
            if (!indices.emplace(mesh.mBones[i]->mName.C_Str(), i).second)
            {
                Log(LogLevel::Error, "Duplicate bone name");
                return false;
            }
        }

        // Sort by depth and name for a consistent parent-first order.
        std::vector<std::pair<unsigned int, std::string>> order;
        for (unsigned int i = 0; i < mesh.mNumBones; i++)
        {
            const aiNode* node = scene.mRootNode->FindNode(mesh.mBones[i]->mName);
            if (node == nullptr)
            {
                Log(LogLevel::Error, "Bone node missing from scene");
                return false;
            }

            unsigned int depth = 0;
            for (node = node->mParent; node != nullptr; node = node->mParent)
            {
                if (indices.count(node->mName.C_Str()) != 0)
                    depth++;
            }

            order.emplace_back(depth, mesh.mBones[i]->mName.C_Str());
        }

        std::sort(order.begin(), order.end());

        for (const std::pair<unsigned int, std::string>& bone : order)
            bones.push_back(mesh.mBones[indices.at(bone.second)]);

        for (unsigned int i = 0; i < bones.size(); i++)
            indices[bones[i]->mName.C_Str()] = i;

        return true;
    }
}
