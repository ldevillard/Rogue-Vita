#pragma once

#include <limits>

namespace dvl
{
    struct ShaderHandle
    {
        static constexpr unsigned int Invalid = std::numeric_limits<unsigned int>::max();

        unsigned int id = Invalid;

        bool IsValid() const
        {
            return id != Invalid;
        }
    };

    struct ShaderCode
    {
        const char* data = nullptr;
        std::size_t size = 0;
    };

    struct ShaderDesc
    {
        ShaderCode vertex;
        ShaderCode fragment;
    };
}