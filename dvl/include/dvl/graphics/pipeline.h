#pragma once

#include "shader.h"

#include <string>
#include <vector>

namespace dvl
{
    struct PipelineHandle
    {
        static constexpr unsigned int Invalid = std::numeric_limits<unsigned int>::max();

        unsigned int id = Invalid;

        bool IsValid() const
        {
            return id != Invalid;
        }
    };

    enum class VertexFormat
    {
        Float3,
        Float4,
    };

    struct VertexAttribute
    {
        const char* name = nullptr;
        VertexFormat format;
        std::size_t offset;
    };

    struct PipelineDesc
    {
        ShaderHandle shader;

        const VertexAttribute* attributes = nullptr;
       
        std::size_t attributeCount = 0;
        std::size_t vertexStride = 0;
    };
}