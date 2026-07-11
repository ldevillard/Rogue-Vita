#pragma once

#include "shader.h"

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

    enum class PrimitiveTopology
    {
        TriangleList
    };

    struct VertexAttribute
    {
        const char* name = nullptr;
        VertexFormat format = VertexFormat::Float3;
        std::size_t offset = 0;
    };

    struct PipelineDesc
    {
        ShaderHandle shader;

        const VertexAttribute* attributes = nullptr;
       
        std::size_t attributeCount = 0;
        std::size_t vertexStride = 0;
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    };
}