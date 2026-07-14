#pragma once

#include <dvl/dvl.h>

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

    dvl::PrimitiveTopology topology = dvl::PrimitiveTopology::TriangleList;
    dvl::DepthStencilState depthStencilState;
    dvl::RasterizerState rasterizerState;
};

struct RenderPipeline
{
    // TODO: Make params open to extension and close to modification (map or vector of params)
    dvl::ShaderHandle shader;
    dvl::PipelineHandle pipeline;

    dvl::ShaderParameterHandle viewProjectionParameter;
    dvl::ShaderParameterHandle modelParameter;

    dvl::ShaderParameterHandle materialColorParameter;

    dvl::ShaderParameterHandle lightCountParameter;
    dvl::ShaderParameterHandle lightDirectionsParameter;
    dvl::ShaderParameterHandle lightColorsParameter;
    dvl::ShaderParameterHandle cameraPositionParameter;

    bool IsValid() const
    {
        return shader.IsValid() && pipeline.IsValid() &&
               viewProjectionParameter.IsValid() && modelParameter.IsValid() &&
               materialColorParameter.IsValid() &&
               lightCountParameter.IsValid() &&
               lightDirectionsParameter.IsValid() &&
               lightColorsParameter.IsValid() &&
               cameraPositionParameter.IsValid();
    }
};
