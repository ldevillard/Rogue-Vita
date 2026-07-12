#pragma once

#include <dvl/dvl.h>

#include <cstddef>
#include <cstdint>

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
