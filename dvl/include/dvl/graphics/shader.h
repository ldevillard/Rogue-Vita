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

    struct ShaderParameterHandle
    {
        static constexpr unsigned int Invalid = std::numeric_limits<unsigned int>::max();

        unsigned int id = Invalid;

        bool IsValid() const
        {
            return id != Invalid;
        }
    };

    enum class ShaderParameterType
    {
        Int,
        Int2,
        Int3,
        Int4,

        Float,
        Float2,
        Float3,
        Float4,

        Mat2,
        Mat3,
        Mat4,
    };

    struct ShaderParameter
    {
        ShaderHandle shader;
        const char* name = nullptr;
        ShaderParameterType type = ShaderParameterType::Float;
    };
}