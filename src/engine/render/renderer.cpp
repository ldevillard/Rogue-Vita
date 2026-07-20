#include "engine/render/renderer.h"

#include "engine/component/camera.h"
#include "engine/component/directional_light.h"
#include "engine/core/asset_registry.h"
#include "engine/core/entity.h"
#include "engine/core/transform.h"
#include "engine/render/material.h"
#include "engine/render/mesh.h"
#include "engine/render/render_pipeline.h"
#include "engine/render/shader_loader.h"

Renderer::Renderer(int screenWidth, int screenHeight, const AssetRegistry& assetRegistry)
    : _assetRegistry(assetRegistry)
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
    }

	_activeCamera = nullptr;
	dvl::Log(dvl::LogLevel::Info, "Graphics device initialized");
}

Renderer::~Renderer()
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
        desc.attributes == nullptr || desc.attributeCount == 0 || desc.vertexStride == 0)
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

    RenderPipeline renderPipeline = {};
    renderPipeline.shader = shader;
    renderPipeline.pipeline = pipeline;

    for (size_t i = 0; i < desc.parameterCount; ++i)
    {
        ShaderParameterDesc parameterDesc = desc.parameters[i];
        dvl::ShaderParameter shaderParameter;

        shaderParameter.name = parameterDesc.name;
        shaderParameter.type = parameterDesc.type;
        shaderParameter.shader = shader;

        const dvl::ShaderParameterHandle parameterHandle = _device.GetShaderParameter(shaderParameter);

        if (!parameterHandle.IsValid())
        {
            dvl::Log(dvl::LogLevel::Error, "Couldn't create shader parameter!");
            return {};
        }

        renderPipeline.parameters.push_back({ parameterDesc.name, parameterHandle });
    }

    return renderPipeline;
}

void Renderer::DestroyRenderPipeline(RenderPipeline& renderPipeline)
{
    for (ShaderParameterBinding& parameter : renderPipeline.parameters)
    {
        _device.DestroyShaderParameter(parameter.handle);
    }

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
    _lightCount = 0;
}

void Renderer::SubmitLight(const DirectionalLight& light)
{
    if (!light.IsValid() || _lightCount >= MaxLights)
    {
        return;
    }

    const glm::vec3 direction = glm::normalize(light.direction);
    _lightDirections[_lightCount] = glm::vec4(direction, 0.0f);
    _lightColors[_lightCount] = glm::vec4(light.color, light.intensity);
    _lightCount++;
}

void Renderer::Draw(const Mesh& mesh, const Material& material, const Transform& transform)
{
    if (_activeCamera == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "There is no active camera, draw call canceled!");
        return;
    }

    const RenderPipeline* renderPipeline = _assetRegistry.GetRenderPipeline(material.renderPipelineHandle);

    if (!mesh.IsValid() || renderPipeline == nullptr || !renderPipeline->IsValid())
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid mesh or material, draw call canceled!");
        return;
    }

    const Entity* cameraEntity = _activeCamera->GetEntity();
    if (cameraEntity == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid active camera entity!");
        return;
    }

    const glm::mat4 modelMatrix = transform.GetMatrix();

    _device.SetPipeline(renderPipeline->pipeline);

    const glm::mat4 viewProjectionMatrix = _activeCamera->GetProjectionMatrix() * _activeCamera->GetViewMatrix();
    setParameter(*renderPipeline, "viewProjectionMatrix", &viewProjectionMatrix[0][0]);

    const glm::vec3 cameraPosition = cameraEntity->transform.position;
    setParameter(*renderPipeline, "cameraPosition", &cameraPosition[0]);

    setParameter(*renderPipeline, "materialColor", &material.color.r);

    setParameter(*renderPipeline, "lightCount", &_lightCount);
    
    if (_lightCount > 0)
    {
        setParameter(*renderPipeline, "lightDirections", &_lightDirections[0][0], _lightCount);
        setParameter(*renderPipeline, "lightColors", &_lightColors[0][0], _lightCount);
    }

    setParameter(*renderPipeline, "modelMatrix", &modelMatrix[0][0]);

    _device.SetVertexBuffer(mesh.vertexBuffer);
    _device.SetIndexBuffer(mesh.indexBuffer);

    _device.DrawIndexed(mesh.indexCount);
}

void Renderer::setParameter(const RenderPipeline& renderPipeline, std::string_view name, const void* data, unsigned int count)
{
    const dvl::ShaderParameterHandle parameter = renderPipeline.GetParameter(name);
    if (parameter.IsValid())
    {
        _device.SetShaderParameter(parameter, data, count);
    }
}
