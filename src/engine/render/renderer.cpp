#include "engine/render/renderer.h"

#include <glm/gtc/type_ptr.hpp>

#include "engine/core/transform.h"
#include "engine/render/camera.h"
#include "engine/render/material.h"
#include "engine/render/mesh.h"
#include "engine/render/render_pipeline.h"
#include "engine/render/shader_loader.h"

bool Renderer::Initialize(int screenWidth, int screenHeight)
{
	dvl::DeviceDesc desc;
	desc.api = dvl::GraphicsAPI::VitaGL;
	desc.width = screenWidth;
	desc.height = screenHeight;
	desc.vsync = true;

	dvl::Log(dvl::LogLevel::Info, "Initializing graphics device");

	if (!_device.Initialize(desc))
	{
		dvl::Log(dvl::LogLevel::Error, "Failed to initialize graphics device");
        return false;
    }

	_activeCamera = nullptr;
	dvl::Log(dvl::LogLevel::Info, "Graphics device initialized");
    return true;
}
    
void Renderer::Shutdown()
{
    _activeCamera = nullptr;
    _device.Shutdown();
}

bool Renderer::CreateMesh(const MeshDesc& desc, Mesh& mesh)
{
    if (mesh.IsValid())
    {
        dvl::Log(dvl::LogLevel::Error, "Cannot overwrite an existing mesh");
        return false;
    }

    if (desc.vertexData == nullptr || desc.vertexDataSize == 0 ||
        desc.indices == nullptr || desc.indexCount == 0)
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid mesh description");
        return false;
    }

    dvl::BufferDesc vertexBufferDesc;
    vertexBufferDesc.type = dvl::BufferType::Vertex;
    vertexBufferDesc.usage = dvl::BufferUsage::Static;
    vertexBufferDesc.size = desc.vertexDataSize;
    vertexBufferDesc.data = desc.vertexData;

    const dvl::BufferHandle vertexBuffer = _device.CreateBuffer(vertexBufferDesc);
    if (!vertexBuffer.IsValid())
    {
        return false;
    }

    dvl::BufferDesc indexBufferDesc;
    indexBufferDesc.type = dvl::BufferType::Index;
    indexBufferDesc.usage = dvl::BufferUsage::Static;
    indexBufferDesc.size = sizeof(std::uint16_t) * desc.indexCount;
    indexBufferDesc.data = desc.indices;

    const dvl::BufferHandle indexBuffer = _device.CreateBuffer(indexBufferDesc);
    if (!indexBuffer.IsValid())
    {
        _device.DestroyBuffer(vertexBuffer);
        return false;
    }

    mesh.vertexBuffer = vertexBuffer;
    mesh.indexBuffer = indexBuffer;
    mesh.indexCount = desc.indexCount;
    return true;
}

void Renderer::DestroyMesh(Mesh& mesh)
{
    _device.DestroyBuffer(mesh.indexBuffer);
    _device.DestroyBuffer(mesh.vertexBuffer);
    mesh = {};
}

RenderPipeline Renderer::CreateRenderPipeline(const RenderPipelineDesc& desc)
{

    if (desc.vertexShaderPath == nullptr || desc.fragmentShaderPath == nullptr ||
        desc.attributes == nullptr || desc.attributeCount == 0 || desc.vertexStride == 0 ||
        desc.mvpParameterName == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid render pipeline description");
        return {};
    }

    ShaderSource shaderSource;
    if (!LoadShaderSource(desc.vertexShaderPath, desc.fragmentShaderPath, shaderSource))
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to load render pipeline shader source");
        return {};
    }

    dvl::ShaderDesc shaderDesc;
    shaderDesc.vertex.data = shaderSource.vertex.c_str();
    shaderDesc.vertex.size = shaderSource.vertex.size();
    shaderDesc.fragment.data = shaderSource.fragment.c_str();
    shaderDesc.fragment.size = shaderSource.fragment.size();

    const dvl::ShaderHandle shader = _device.CreateShader(shaderDesc);
    if (!shader.IsValid())
    {
        return {};
    }

    dvl::PipelineDesc pipelineDesc;
    pipelineDesc.shader = shader;
    pipelineDesc.attributes = desc.attributes;
    pipelineDesc.attributeCount = desc.attributeCount;
    pipelineDesc.vertexStride = desc.vertexStride;
    pipelineDesc.topology = desc.topology;
    pipelineDesc.depthStencilState = desc.depthStencilState;
    pipelineDesc.rasterizerState = desc.rasterizerState;

    const dvl::PipelineHandle pipeline = _device.CreatePipeline(pipelineDesc);
    if (!pipeline.IsValid())
    {
        _device.DestroyShader(shader);
        return {};
    }

    dvl::ShaderParameter parameterDesc;
    parameterDesc.shader = shader;
    parameterDesc.name = desc.mvpParameterName;
    parameterDesc.type = dvl::ShaderParameterType::Mat4;

    const dvl::ShaderParameterHandle mvpParameter = _device.GetShaderParameter(parameterDesc);
    if (!mvpParameter.IsValid())
    {
        _device.DestroyPipeline(pipeline);
        _device.DestroyShader(shader);
        return {};
    }

    return {shader, pipeline, mvpParameter};
}

void Renderer::DestroyRenderPipeline(RenderPipeline& renderPipeline)
{
    _device.DestroyShaderParameter(renderPipeline.mvpParameter);
    _device.DestroyPipeline(renderPipeline.pipeline);
    _device.DestroyShader(renderPipeline.shader);
    renderPipeline = {};
}

void Renderer::BeginFrame(const dvl::Color& clearColor)
{
    _device.BeginFrame(clearColor);
}

void Renderer::EndFrame()
{
    _device.EndFrame();
}

void Renderer::BeginScene(const Camera& camera)
{
    _activeCamera = &camera;
}

void Renderer::Draw(const Mesh& mesh, 
            const Material& material, 
            const Transform& transform)
{
    if (_activeCamera == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "There is no active camera, draw call canceled!");
        return;
    }

    if (!mesh.IsValid() || !material.pipeline.IsValid())
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid mesh or material, draw call canceled!");
        return;
    }

    const RenderPipeline& renderPipeline = material.pipeline;
    const glm::mat4 modelMatrix = transform.GetMatrix();

    const glm::mat4 mvpMatrix = _activeCamera->GetProjectionMatrix() *
                                _activeCamera->GetViewMatrix() *
                                modelMatrix;

    _device.SetPipeline(renderPipeline.pipeline);

    _device.SetShaderParameter(renderPipeline.mvpParameter, glm::value_ptr(mvpMatrix), 1);

    _device.SetVertexBuffer(mesh.vertexBuffer);
    _device.SetIndexBuffer(mesh.indexBuffer);

    _device.DrawIndexed(mesh.indexCount);
}
