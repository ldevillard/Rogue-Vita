#pragma once

#include <limits>

namespace dvl
{
    struct TextureHandle
    {
         static constexpr unsigned int Invalid = std::numeric_limits<unsigned int>::max();

        unsigned int id = Invalid;

        bool IsValid() const
        {
            return id != Invalid;
        }
    };
    
    struct TextureDesc
    {
        unsigned int width = 0;
        unsigned int height = 0;
        
        const void* data = nullptr;
    };
}