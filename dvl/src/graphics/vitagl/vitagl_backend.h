#pragma once

#include "../internal/backend.h"

#include <unordered_map>

namespace dvl::internal
{
    class VitaGLBackend : public Backend
    {
    public:
        bool Initialize(const DeviceDesc& desc) override;
        void Shutdown() override;

        void BeginFrame(const Color& clearColor) override;
        void EndFrame() override;

        void SetViewport(const Viewport& viewport) override;

        BufferHandle CreateBuffer(const BufferDesc& desc) override;
        void DestroyBuffer(BufferHandle handle) override;

        ShaderHandle CreateShader(const ShaderDesc& desc) override;
        void DestroyShader(ShaderHandle handle) override;

    private:
        // -- BUFFER --

        struct NativeBuffer
        {
            unsigned int id = 0;

            std::size_t size = 0;
        };

        // Use a vector of generational slots if resource management needs to scale
        std::unordered_map<unsigned int, NativeBuffer> _buffers;
        unsigned int _nextBufferHandle = 0;

        // -- SHADER --

        struct NativeShader
        {
            unsigned int program = 0;
        };

        // Use a vector of generational slots if resource management needs to scale
        std::unordered_map<unsigned int, NativeShader> _shaders;
        unsigned int _nextShaderHandle = 0;
    };
}
