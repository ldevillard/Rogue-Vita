#pragma once

#include <functional>
#include <limits>

struct TextureHandle
{
    static constexpr unsigned int Invalid = std::numeric_limits<unsigned int>::max();
    
    unsigned int id = Invalid;
    
    bool IsValid() const
    {
        return id != Invalid;
    }

    bool operator==(const TextureHandle& other) const
    {
        return id == other.id;
    }

    struct Hasher
    {
        std::size_t operator()(const TextureHandle& handle) const
        {
            return std::hash<unsigned int>{}(handle.id);
        }
    };
};

struct TextureDesc
{
    unsigned int width = 0;
    unsigned int height = 0;

    const void* data = nullptr;
};

struct Texture
{
    // This the texture id sent to the graphic backend, not the handle
    unsigned int id = TextureHandle::Invalid;

    bool IsValid() const
    {
        return id != TextureHandle::Invalid;
    }
};
