#pragma once

#include <limits>

namespace dvl
{
    struct BufferHandle
    {
        static constexpr unsigned int Invalid = std::numeric_limits<unsigned int>::max();

        unsigned int id = Invalid;

        bool IsValid() const
        {
            return id != Invalid;
        }
    };

    enum class BufferType
    {
        Vertex,
        Index
    };

    enum class BufferUsage
    {
        Static,
        Dynamic
    };

    struct BufferDesc
    {
        BufferType type = BufferType::Vertex;
        BufferUsage usage = BufferUsage::Static;

        std::size_t size = 0;

        const void* data = nullptr;
    };
}
