#pragma once

#include <dvl/anim/animation.h>

#include <functional>
#include <limits>
#include <vector>

struct AnimationHandle
{
    static constexpr unsigned int Invalid = std::numeric_limits<unsigned int>::max();
    unsigned int id = Invalid;

    bool IsValid() const { return id != Invalid; }
    bool operator==(const AnimationHandle& other) const { return id == other.id; }

    struct Hasher
    {
        std::size_t operator()(const AnimationHandle& handle) const
        {
            return std::hash<unsigned int>{}(handle.id);
        }
    };
};

struct Animation
{
    float duration = 0.0f;
    float fps = 0.0f;
    int boneCount = 0;
    int frameCount = 0;
    std::vector<dvl::Transform> keyframes;

    bool IsValid() const
    {
        return duration > 0.0f && fps > 0.0f && boneCount > 0 && frameCount > 0 &&
               keyframes.size() == static_cast<std::size_t>(boneCount * frameCount);
    }

    dvl::Animation GetView() const
    {
        return {duration, fps, boneCount, frameCount, keyframes.data()};
    }
};
