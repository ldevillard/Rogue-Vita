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

    dvl::ShaderParameter parameterDesc;
    parameterDesc.shader = shader;
    parameterDesc.name = "viewProjectionMatrix";
    parameterDesc.type = dvl::ShaderParameterType::Mat4;

    const dvl::ShaderParameterHandle viewProjectionParameter = _device.GetShaderParameter(parameterDesc);
    if (!viewProjectionParameter.IsValid())
    {
        _device.DestroyPipeline(pipeline);
        _device.DestroyShader(shader);
        return {};
    }

    parameterDesc.name = "modelMatrix";
    parameterDesc.type = dvl::ShaderParameterType::Mat4;
    const dvl::ShaderParameterHandle modelParameter = _device.GetShaderParameter(parameterDesc);

    parameterDesc.name = "materialColor";
    parameterDesc.type = dvl::ShaderParameterType::Float4;
    const dvl::ShaderParameterHandle materialColorParameter = _device.GetShaderParameter(parameterDesc);

    parameterDesc.name = "lightCount";
    parameterDesc.type = dvl::ShaderParameterType::Int;
    const dvl::ShaderParameterHandle lightCountParameter = _device.GetShaderParameter(parameterDesc);

    parameterDesc.name = "lightDirections";
    parameterDesc.type = dvl::ShaderParameterType::Float4;
    const dvl::ShaderParameterHandle lightDirectionsParameter = _device.GetShaderParameter(parameterDesc);

    parameterDesc.name = "lightColors";
    parameterDesc.type = dvl::ShaderParameterType::Float4;
    const dvl::ShaderParameterHandle lightColorsParameter = _device.GetShaderParameter(parameterDesc);

    parameterDesc.name = "cameraPosition";
    parameterDesc.type = dvl::ShaderParameterType::Float3;
    const dvl::ShaderParameterHandle cameraPositionParameter = _device.GetShaderParameter(parameterDesc);

    RenderPipeline renderPipeline = {};
    renderPipeline.shader = shader;
    renderPipeline.pipeline = pipeline;
    renderPipeline.viewProjectionParameter = viewProjectionParameter;
    renderPipeline.modelParameter = modelParameter;
    renderPipeline.materialColorParameter = materialColorParameter;
    renderPipeline.lightCountParameter = lightCountParameter;
    renderPipeline.lightDirectionsParameter = lightDirectionsParameter;
    renderPipeline.lightColorsParameter = lightColorsParameter;
    renderPipeline.cameraPositionParameter = cameraPositionParameter;

    return renderPipeline;
}

void Renderer::DestroyRenderPipeline(RenderPipeline& renderPipeline)
{
    _device.DestroyShaderParameter(renderPipeline.cameraPositionParameter);
    _device.DestroyShaderParameter(renderPipeline.materialColorParameter);
    _device.DestroyShaderParameter(renderPipeline.lightCountParameter);
    _device.DestroyShaderParameter(renderPipeline.lightColorsParameter);
    _device.DestroyShaderParameter(renderPipeline.lightDirectionsParameter);
    _device.DestroyShaderParameter(renderPipeline.modelParameter);
    _device.DestroyShaderParameter(renderPipeline.viewProjectionParameter);
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
    _device.SetShaderParameter(renderPipeline->viewProjectionParameter, &viewProjectionMatrix[0][0], 1);

    const glm::vec3 cameraPosition = cameraEntity->transform.position;
    _device.SetShaderParameter(renderPipeline->cameraPositionParameter, &cameraPosition[0], 1);

    _device.SetShaderParameter(renderPipeline->materialColorParameter, &material.color.r, 1);

    _device.SetShaderParameter(renderPipeline->lightCountParameter, &_lightCount, 1);
    
    if (_lightCount > 0)
    {
        _device.SetShaderParameter(renderPipeline->lightDirectionsParameter, &_lightDirections[0][0], _lightCount);
        _device.SetShaderParameter(renderPipeline->lightColorsParameter, &_lightColors[0][0], _lightCount);
    }

    _device.SetShaderParameter(renderPipeline->modelParameter, &modelMatrix[0][0], 1);

    _device.SetVertexBuffer(mesh.vertexBuffer);
    _device.SetIndexBuffer(mesh.indexBuffer);

    _device.DrawIndexed(mesh.indexCount);
}
