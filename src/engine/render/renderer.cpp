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

bool Renderer::CreateRenderPipeline(const RenderPipelineDesc& desc, RenderPipeline& renderPipeline)
{
    if (renderPipeline.IsValid())
    {
        dvl::Log(dvl::LogLevel::Error, "Cannot overwrite an existing render pipeline");
        return false;
    }

    if (desc.vertexShaderPath == nullptr || desc.fragmentShaderPath == nullptr ||
        desc.attributes == nullptr || desc.attributeCount == 0 || desc.vertexStride == 0)
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid render pipeline description");
        return false;
    }

    ShaderSource shaderSource;
    if (!LoadShaderSource(desc.vertexShaderPath, desc.fragmentShaderPath, shaderSource))
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to load render pipeline shader source");
        return false;
    }

    dvl::ShaderDesc shaderDesc;
    shaderDesc.vertex.data = shaderSource.vertex.c_str();
    shaderDesc.vertex.size = shaderSource.vertex.size();
    shaderDesc.fragment.data = shaderSource.fragment.c_str();
    shaderDesc.fragment.size = shaderSource.fragment.size();

    const dvl::ShaderHandle shader = _device.CreateShader(shaderDesc);
    if (!shader.IsValid())
    {
        return false;
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
        return false;
    }

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
            DestroyRenderPipeline(renderPipeline);
            return false;
        }

        renderPipeline.parameters.push_back({ parameterDesc.name, parameterHandle });
    }

    return true;
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

bool Renderer::CreateTexture(const TextureDesc& desc, Texture& texture)
{
    if (texture.IsValid())
    {
        dvl::Log(dvl::LogLevel::Error, "Cannot overwrite an existing texture");
        return false;
    }

    dvl::TextureDesc textureDesc = {};
    textureDesc.width = desc.width;
    textureDesc.height = desc.height;
    textureDesc.data = desc.data;

    const dvl::TextureHandle textureHandle = _device.CreateTexture(textureDesc);

    if (!textureHandle.IsValid())
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to create texture!");
        return false;
    }

    texture.id = textureHandle.id;
    return true;
}

void Renderer::DestroyTexture(Texture& texture)
{
    dvl::TextureHandle textureHandle = { texture.id };
    _device.DestroyTexture(textureHandle);
    texture = {};
}

void Renderer::BeginFrame(const glm::vec4& clearColor)
{
    _device.BeginFrame(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
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
    if (_lightCount >= MaxLights)
    {
        return;
    }

    const glm::vec3 direction = glm::normalize(light.direction);
    _lightDirections[_lightCount] = glm::vec4(direction, 0.0f);
    _lightColors[_lightCount] = glm::vec4(light.color, light.intensity);
    _lightCount++;
}

void Renderer::Draw(const Mesh& mesh, const Material& material, const glm::mat4& modelMatrix)
{
    if (_activeCamera == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "There is no active camera, draw call canceled!");
        return;
    }

    const RenderPipeline* renderPipeline = _assetRegistry.GetRenderPipeline(material.renderPipelineHandle);
    const Texture* texture = _assetRegistry.GetTexture(material.textureHandle);

    if (!mesh.IsValid() || renderPipeline == nullptr || !renderPipeline->IsValid() ||
        texture == nullptr)
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

    _device.SetPipeline(renderPipeline->pipeline);

    // Parameters binding
    {
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

        _device.SetTexture({ texture->id });
        const int textureSlot = 0;
        setParameter(*renderPipeline, "albedoTexture", &textureSlot);
    }

    _device.SetVertexBuffer(mesh.vertexBuffer);
    _device.SetIndexBuffer(mesh.indexBuffer);

    _device.DrawIndexed(mesh.indexCount);
}

void Renderer::setParameter(const RenderPipeline& renderPipeline, const char* name, const void* data, unsigned int count)
{
    const dvl::ShaderParameterHandle parameter = renderPipeline.GetParameter(name);
    _device.SetShaderParameter(parameter, data, count);
}
