#pragma once

#include <dvl/dvl.h>

#include <cstddef>
#include <cstdint>

struct MeshHandle
{
    static constexpr unsigned int Invalid = std::numeric_limits<unsigned int>::max();
    
    unsigned int id = Invalid;
    
    bool IsValid() const
    {
        return id != Invalid;
    }

    bool operator==(const MeshHandle& other) const
    {
        return id == other.id;
    }

    struct Hasher
    {
        std::size_t operator()(const MeshHandle& handle) const
        {
            return std::hash<unsigned int>{}(handle.id);
        }
    };
};

struct MeshDesc
{
    const void* vertexData = nullptr;
    std::size_t vertexDataSize = 0;

    const std::uint16_t* indices = nullptr;
    unsigned int indexCount = 0;
};

struct Mesh 
{
    dvl::BufferHandle vertexBuffer;
    dvl::BufferHandle indexBuffer;

    unsigned int indexCount = 0;

    bool IsValid() const
    {
        return vertexBuffer.IsValid() && indexBuffer.IsValid() && indexCount > 0;
    }
};
