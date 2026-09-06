#pragma once

#include <dvl/dvl.h>

#include <cstddef>
#include <cstdint>

struct SkeletonHandle
{
    static constexpr unsigned int Invalid = std::numeric_limits<unsigned int>::max();
    
    unsigned int id = Invalid;
    
    bool IsValid() const
    {
        return id != Invalid;
    }

    bool operator==(const SkeletonHandle& other) const
    {
        return id == other.id;
    }

    struct Hasher
    {
        std::size_t operator()(const SkeletonHandle& handle) const
        {
            return std::hash<unsigned int>{}(handle.id);
        }
    };
};

struct Skeleton 
{
    std::vector<std::int16_t> parents;
    std::vector<dvl::Mat4> inverseBindMatrices;

    int boneCount = 0;

    bool IsValid() const
    {
        return boneCount > 0 && 
                parents.size() == static_cast<size_t>(boneCount) && 
                inverseBindMatrices.size() == static_cast<size_t>(boneCount);
    }
};
