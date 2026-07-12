#pragma once

#include <dvl/dvl.h>

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

    const char* mvpParameterName = "mvpMatrix";
};

struct RenderPipeline
{
    dvl::ShaderHandle shader;
    dvl::PipelineHandle pipeline;

    dvl::ShaderParameterHandle mvpParameter;

    bool IsValid() const
    {
        return shader.IsValid() && pipeline.IsValid() && mvpParameter.IsValid();
    }
};
