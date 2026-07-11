#pragma once

#include "buffer.h"
#include "shader.h"
#include "pipeline.h"
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

        void BeginFrame(const Color& clearColor);
        void EndFrame();

        bool IsInitialized() const;

        void SetViewport(const Viewport& viewport);

        BufferHandle CreateBuffer(const BufferDesc& desc);
        void DestroyBuffer(BufferHandle handle);

        ShaderHandle CreateShader(const ShaderDesc& desc);
        void DestroyShader(ShaderHandle handle);

        PipelineHandle CreatePipeline(const PipelineDesc& desc);
        void DestroyPipeline(PipelineHandle handle);

        void SetPipeline(PipelineHandle handle);
        void SetVertexBuffer(BufferHandle handle);
        
        void Draw(unsigned int vertexCount);

    private:
        std::unique_ptr<internal::Backend> _backend;
    };
}
