#include "dvl/tool/cooker/animation_cooker.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <cmath>
#include <fstream>
#include <vector>

#include "dvl/asset/animation_format.h"
#include "dvl/log/log.h"
#include "dvl/tool/cooker/asset_space_cooker_helper.h"

namespace dvl
{
    namespace
    {
        constexpr float AnimationFps = 30.0f;

        const aiNodeAnim* FindChannel(const aiAnimation& animation, const aiString& nodeName)
        {
            for (unsigned int i = 0; i < animation.mNumChannels; i++)
            {
                if (animation.mChannels[i]->mNodeName == nodeName)
                    return animation.mChannels[i];
            }

            return nullptr;
        }

        aiVector3D Sample(const aiVectorKey* keys, unsigned int count, double time)
        {
            if (time <= keys[0].mTime)
                return keys[0].mValue;

            for (unsigned int i = 1; i < count; i++)
            {
                if (time < keys[i].mTime)
                {
                    const float alpha = static_cast<float>((time - keys[i - 1].mTime) / (keys[i].mTime - keys[i - 1].mTime));
                    return keys[i - 1].mValue + (keys[i].mValue - keys[i - 1].mValue) * alpha;
                }
            }

            return keys[count - 1].mValue;
        }

        aiQuaternion Sample(const aiQuatKey* keys, unsigned int count, double time)
        {
            if (time <= keys[0].mTime)
                return keys[0].mValue;

            for (unsigned int i = 1; i < count; i++)
            {
                if (time < keys[i].mTime)
                {
                    const float alpha = static_cast<float>((time - keys[i - 1].mTime) / (keys[i].mTime - keys[i - 1].mTime));
                    aiQuaternion result;
                    aiQuaternion::Interpolate(result, keys[i - 1].mValue, keys[i].mValue, alpha);
                    return result.Normalize();
                }
            }
            return keys[count - 1].mValue;
        }

        aiMatrix4x4 SampleLocalTransform(const aiNode& node, const aiAnimation& animation, double time)
        {
            const aiNodeAnim* channel = FindChannel(animation, node.mName);
            if (channel == nullptr)
                return node.mTransformation;

            const aiVector3D translation = Sample(channel->mPositionKeys, channel->mNumPositionKeys, time);
            const aiQuaternion rotation = Sample(channel->mRotationKeys, channel->mNumRotationKeys, time);
            const aiVector3D scale = Sample(channel->mScalingKeys, channel->mNumScalingKeys, time);

            return aiMatrix4x4(scale, rotation, translation);
        }

        bool IsBone(const aiMesh& mesh, const aiNode* node)
        {
            if (node == nullptr)
                return false;

            for (unsigned int i = 0; i < mesh.mNumBones; i++)
            {
                if (mesh.mBones[i]->mName == node->mName)
                    return true;
            }

            return false;
        }

        aiMatrix4x4 GetRootToCookedTransform(const aiScene& scene, const aiMesh& mesh)
        {
            AssetSpaceCookerHelper assetSpace;
            assetSpace.Initialize(scene);

            aiMatrix4x4 meshTransform;

            for (const aiNode* node = scene.mRootNode->FindNode(mesh.mName); node != nullptr; node = node->mParent)
            {
                meshTransform = node->mTransformation * meshTransform;
            }

            return assetSpace.ToSourceMatrix().Inverse() * meshTransform.Inverse();
        }

        Transform SampleBoneTransform(const aiNode& boneNode, const aiMesh& mesh, const aiAnimation& animation, double time, const aiMatrix4x4& rootToCooked)
        {
            aiMatrix4x4 transform = SampleLocalTransform(boneNode, animation, time);

            // Parents absent from the skinning palette are baked into skeleton roots.
            if (!IsBone(mesh, boneNode.mParent))
            {
                for (const aiNode* parent = boneNode.mParent; parent != nullptr; parent = parent->mParent)
                {
                    transform = SampleLocalTransform(*parent, animation, time) * transform;
                }

                transform = rootToCooked * transform;
            }

            aiVector3D scale;
            aiVector3D translation;
            aiQuaternion rotation;
            transform.Decompose(scale, rotation, translation);

            Transform result;
            result.translation = Vec4(translation.x, translation.y, translation.z, 0.0f);
            result.rotation = Quat(rotation.x, rotation.y, rotation.z, rotation.w);
            result.scale = Vec4(scale.x, scale.y, scale.z, 0.0f);

            return result;
        }
    }

    bool AnimationCooker::Cook(const std::filesystem::path& source, const std::filesystem::path& destination) const
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(source.string(), 0);
        if (scene == nullptr)
        {
            const std::string message = "Failed to import animation '" + source.string() + "': " + importer.GetErrorString();
            Log(LogLevel::Error, message.c_str());
            return false;
        }

        if (!scene->HasAnimations())
            return true;

        const aiMesh& mesh = *scene->mMeshes[0];
        const aiAnimation& animation = *scene->mAnimations[0];
        const double ticksPerSecond = animation.mTicksPerSecond > 0.0 ? animation.mTicksPerSecond : AnimationFps;
        const float duration = static_cast<float>(animation.mDuration / ticksPerSecond);
        const std::uint32_t frameCount = static_cast<std::uint32_t>(std::ceil(duration * AnimationFps)) + 1;
        const aiMatrix4x4 rootToCooked = GetRootToCookedTransform(*scene, mesh);

        std::vector<Transform> keyframes(static_cast<std::size_t>(frameCount) * mesh.mNumBones);
        for (std::uint32_t frame = 0; frame < frameCount; frame++)
        {
            const double time = frame / AnimationFps * ticksPerSecond;

            for (unsigned int bone = 0; bone < mesh.mNumBones; bone++)
            {
                const aiNode* boneNode = scene->mRootNode->FindNode(mesh.mBones[bone]->mName);
                keyframes[frame * mesh.mNumBones + bone] = SampleBoneTransform(*boneNode, mesh, animation, time, rootToCooked);
            }
        }

        const AnimationFileHeader header =
        {
            AnimationMagic,
            AnimationVersion,

            duration,
            AnimationFps,

            mesh.mNumBones,
            frameCount
        };

        std::ofstream output(destination, std::ios::binary);
        output.write(reinterpret_cast<const char*>(&header), sizeof(header));
        output.write(reinterpret_cast<const char*>(keyframes.data()), keyframes.size() * sizeof(Transform));

        const std::string message = "Cooked " + source.string() + " -> " + destination.string();
        Log(LogLevel::Info, message.c_str());

        return true;
    }

    std::string AnimationCooker::GetOutputExtension() const
    {
        return ".dvlanim";
    }
}
