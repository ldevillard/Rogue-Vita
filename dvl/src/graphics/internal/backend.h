#pragma once 

#include <dvl/graphics/buffer.h>
#include <dvl/graphics/shader.h>
#include <dvl/graphics/types.h>

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
    };
}
