#pragma once 

#include "dvl/graphics/buffer.h"
#include "dvl/graphics/shader.h"
#include "dvl/graphics/pipeline.h"
#include "dvl/graphics/types.h"

namespace dvl::internal
{
    class Backend
    {
    public:
        virtual ~Backend() = default;

        virtual bool Initialize(const DeviceDesc& desc) = 0;
        virtual void Shutdown() = 0;

        virtual void BeginFrame(const Color& clearColor) = 0;
        virtual void EndFrame() = 0;

        virtual void SetViewport(const Viewport& viewport) = 0;

        virtual BufferHandle CreateBuffer(const BufferDesc& desc) = 0;
        virtual void DestroyBuffer(BufferHandle handle) = 0;

        virtual ShaderHandle CreateShader(const ShaderDesc& desc) = 0;
        virtual void DestroyShader(ShaderHandle handle) = 0;

        virtual PipelineHandle CreatePipeline(const PipelineDesc& desc) = 0;
        virtual void DestroyPipeline(PipelineHandle handle) = 0;

        virtual void SetPipeline(PipelineHandle handle) = 0;
        virtual void SetVertexBuffer(BufferHandle handle) = 0;
        virtual void SetIndexBuffer(BufferHandle handle) = 0;

        virtual void Draw(unsigned int vertexCount) = 0;
        virtual void DrawIndexed(unsigned int indexCount) = 0;

        virtual ShaderParameterHandle GetShaderParameter(const ShaderParameter& desc) = 0;
        virtual void DestroyShaderParameter(ShaderParameterHandle handle) = 0;
        virtual void SetShaderParameter(ShaderParameterHandle handle, const void* data, unsigned int count) = 0;
    };
}
