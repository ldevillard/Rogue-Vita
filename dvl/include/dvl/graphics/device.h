#pragma once

#include "buffer.h"
#include "shader.h"
#include "pipeline.h"
#include "texture.h"
#include "types.h"

#include <memory>

namespace dvl::internal
{
    class Backend;
}

namespace dvl
{
    class Device
    {
    public:
        Device();
        ~Device();

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        bool Initialize(const DeviceDesc& desc);
        void Shutdown();

        void BeginFrame(float r, float g, float b, float a);
        void EndFrame();

        bool IsInitialized() const;

        void SetViewport(const Viewport& viewport);

        BufferHandle CreateBuffer(const BufferDesc& desc);
        void DestroyBuffer(BufferHandle handle);

        ShaderHandle CreateShader(const ShaderDesc& desc);
        void DestroyShader(ShaderHandle handle);

        PipelineHandle CreatePipeline(const PipelineDesc& desc);
        void DestroyPipeline(PipelineHandle handle);

        TextureHandle CreateTexture(const TextureDesc& desc);
        void DestroyTexture(TextureHandle handle);

        void SetPipeline(PipelineHandle handle);
        void SetVertexBuffer(BufferHandle handle);
        void SetIndexBuffer(BufferHandle handle);
        void SetTexture(TextureHandle handle);
        
        void Draw(unsigned int vertexCount);
        void DrawIndexed(unsigned int indexCount);

        ShaderParameterHandle GetShaderParameter(const ShaderParameter& desc);
        void DestroyShaderParameter(ShaderParameterHandle handle);
        void SetShaderParameter(ShaderParameterHandle handle, const void* data, unsigned int count);

    private:
        std::unique_ptr<internal::Backend> _backend;
    };
}
