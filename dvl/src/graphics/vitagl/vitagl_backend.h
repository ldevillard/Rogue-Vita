#pragma once

#include "../internal/backend.h"

#include <unordered_map>
#include <vector>

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

        PipelineHandle CreatePipeline(const PipelineDesc& desc) override;
        void DestroyPipeline(PipelineHandle handle) override;

        void SetPipeline(PipelineHandle handle) override;
        void SetVertexBuffer(BufferHandle handle) override;

        // If VBO CPU overhead becomes an issue, think about VAO and caching the vertex attribute state
        void Draw(unsigned int vertexCount) override;

        ShaderParameterHandle GetShaderParameter(const ShaderParameter& desc) override;
        void DestroyShaderParameter(ShaderParameterHandle handle) override;
        void SetShaderParameter(ShaderParameterHandle handle, const void* data, unsigned int count) override;

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

        // -- PIPELINE --

        struct NativePipeline
        {
            struct NativeVertexAttribute
            {
                int location = -1;
                int componentCount = 0;
                std::size_t offset = 0;
            };

            unsigned int program = 0;

            std::vector<NativeVertexAttribute> attributes;
            std::size_t vertexStride = 0;
            PrimitiveTopology topology = PrimitiveTopology::TriangleList;

            DepthStencilState depthStencilState;
            RasterizerState rasterizerState;
        };

        std::unordered_map<unsigned int, NativePipeline> _pipelines;
        PipelineHandle _currentPipeline;
        unsigned int _nextPipelineHandle = 0;

        // -- SHADER PARAMETERS --

        struct NativeShaderParameter
        {
            int location = -1;
            ShaderParameterType type = ShaderParameterType::Float;
        };

        std::unordered_map<unsigned int, NativeShaderParameter> _shaderParameters;
        unsigned int _nextShaderParameterHandle = 0;
    };
}
