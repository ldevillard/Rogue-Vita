#pragma once

#include <dvl/dvl.h>

#include <string_view>
#include <vector>

#include "shader_parameter.h"

struct RenderPipelineHandle
{
    static constexpr unsigned int Invalid = std::numeric_limits<unsigned int>::max();
    
    unsigned int id = Invalid;
    
    bool IsValid() const
    {
        return id != Invalid;
    }

    bool operator==(const RenderPipelineHandle& other) const
    {
        return id == other.id;
    }

    struct Hasher
    {
        std::size_t operator()(const RenderPipelineHandle& handle) const
        {
            return std::hash<unsigned int>{}(handle.id);
        }
    };
};

struct RenderPipelineDesc
{
    const char* vertexShaderPath = nullptr;
    const char* fragmentShaderPath = nullptr;

    const dvl::VertexAttribute* attributes = nullptr;
    std::size_t attributeCount = 0;
    std::size_t vertexStride = 0;

    const ShaderParameterDesc* parameters = nullptr;
    std::size_t parameterCount = 0;

    dvl::PrimitiveTopology topology = dvl::PrimitiveTopology::TriangleList;
    dvl::DepthStencilState depthStencilState;
    dvl::RasterizerState rasterizerState;
};

struct RenderPipeline
{
    dvl::ShaderHandle shader;
    dvl::PipelineHandle pipeline;

    std::vector<ShaderParameterBinding> parameters;

    dvl::ShaderParameterHandle GetParameter(std::string_view name) const;

    bool IsValid() const;
};
