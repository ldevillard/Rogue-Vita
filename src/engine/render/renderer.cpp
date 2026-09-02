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
#if defined (__vita__)
	desc.api = dvl::GraphicsAPI::VitaGL;
#else
    desc.api = dvl::GraphicsAPI::OpenGL;
#endif
    
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
    vertexBufferDesc.usage = desc.vertexBufferUsage;
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

bool Renderer::UpdateMeshVertices(const Mesh& mesh, const void* vertexData, std::size_t vertexDataSize)
{
    if (!mesh.IsValid())
    {
        dvl::Log(dvl::LogLevel::Error, "Cannot update an invalid mesh");
        return false;
    }

    return _device.UpdateBuffer(mesh.vertexBuffer, vertexData, vertexDataSize);
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

    if (desc.vertexShaderPath.empty() || desc.fragmentShaderPath.empty() ||
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

        renderPipeline.parameters.push_back({ parameterDesc.type, parameterDesc.semantic, parameterHandle });
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

void Renderer::BeginFrame(const dvl::Vec4& clearColor)
{
    _device.BeginFrame(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
}

void Renderer::EndFrame()
{
    _device.EndFrame();
}

bool Renderer::ShouldClose() const
{
    return _device.ShouldClose();
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

    const dvl::Vec3 direction = light.direction.Normalized();
    _lightDirections[_lightCount] = dvl::Vec4(direction.x, direction.y, direction.z, 0.0f);
    _lightColors[_lightCount] = dvl::Vec4(light.color.x, light.color.y, light.color.z, light.intensity);
    _lightCount++;
}

void Renderer::Draw(const Mesh& mesh, const Material& material, const dvl::Mat4& modelMatrix)
{
    if (_activeCamera == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "There is no active camera, draw call canceled!");
        return;
    }

    const RenderPipeline* renderPipeline = _assetRegistry.GetRenderPipeline(material.materialTemplate.staticPipeline);

    if (!mesh.IsValid() || renderPipeline == nullptr || !renderPipeline->IsValid())
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid mesh or material, draw call canceled!");
        return;
    }

    _device.SetPipeline(renderPipeline->pipeline);

    DrawContext context;
    context.camera = _activeCamera;
    context.material = &material;
    context.modelMatrix = &modelMatrix;

    for (const ShaderParameterBinding& parameter : renderPipeline->parameters)
    {
        bindParameter(parameter, context);
    }

    _device.SetVertexBuffer(mesh.vertexBuffer);
    _device.SetIndexBuffer(mesh.indexBuffer);

    _device.DrawIndexed(mesh.indexCount);
}

void Renderer::DrawSkinned(const Mesh& mesh, const Material& material, const dvl::Mat4& modelMatrix, const dvl::Mat4* skinningMatrices, int boneCount)
{
    if (_activeCamera == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "There is no active camera, draw call canceled!");
        return;
    }

    if (skinningMatrices == nullptr || boneCount == 0 || boneCount > MaxBones)
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid skinning matrices");
        return;
    }

    const RenderPipeline* renderPipeline = _assetRegistry.GetRenderPipeline(material.materialTemplate.skinnedPipeline);

    if (!mesh.IsValid() || renderPipeline == nullptr || !renderPipeline->IsValid())
    {
        dvl::Log(dvl::LogLevel::Error, "Invalid mesh or material, draw call canceled!");
        return;
    }

    _device.SetPipeline(renderPipeline->pipeline);

    DrawContext context;
    context.camera = _activeCamera;
    context.material = &material;
    context.modelMatrix = &modelMatrix;
    context.skinningMatrices = skinningMatrices;
    context.boneCount = static_cast<std::uint32_t>(boneCount);

    for (const ShaderParameterBinding& parameter : renderPipeline->parameters)
    {
        bindParameter(parameter, context);
    }

    _device.SetVertexBuffer(mesh.vertexBuffer);
    _device.SetIndexBuffer(mesh.indexBuffer);

    _device.DrawIndexed(mesh.indexCount);
}

void Renderer::bindParameter(const ShaderParameterBinding& parameter, const DrawContext& context)
{
    switch (parameter.semantic)
    {
        case ShaderParameterSemantic::ViewProjection:
        {
            if (context.camera == nullptr)
                return;

            const dvl::Mat4 viewProjectionMatrix = context.camera->GetProjectionMatrix() * context.camera->GetViewMatrix();
            _device.SetShaderParameter(parameter.handle, &viewProjectionMatrix[0][0], 1);
            break;
        }

        case ShaderParameterSemantic::CameraPosition:
        {
            if (context.camera == nullptr || context.camera->GetEntity() == nullptr)
                return;

            const dvl::Vec3& cameraPosition = context.camera->GetEntity()->transform.position;
            _device.SetShaderParameter(parameter.handle, &cameraPosition.x, 1);
            break;
        }

        case ShaderParameterSemantic::ModelMatrix:
            if (context.modelMatrix != nullptr)
                _device.SetShaderParameter(parameter.handle, &(*context.modelMatrix)[0][0], 1);
            break;

        case ShaderParameterSemantic::MaterialColor:
            if (context.material != nullptr)
                _device.SetShaderParameter(parameter.handle, &context.material->color.x, 1);
            break;

        case ShaderParameterSemantic::AlbedoTexture:
        {
            if (context.material == nullptr)
                return;

            const Texture* texture = _assetRegistry.GetTexture(context.material->textureHandle);
            if (texture == nullptr)
            {
                dvl::Log(dvl::LogLevel::Error, "Invalid albedo texture");
                return;
            }

            _device.SetTexture({ texture->id });
            const int textureSlot = 0;
            _device.SetShaderParameter(parameter.handle, &textureSlot, 1);
            break;
        }

        case ShaderParameterSemantic::LightCount:
            _device.SetShaderParameter(parameter.handle, &_lightCount, 1);
            break;

        case ShaderParameterSemantic::LightDirections:
            if (_lightCount > 0)
                _device.SetShaderParameter(parameter.handle, &_lightDirections[0].x, _lightCount);
            break;

        case ShaderParameterSemantic::LightColors:
            if (_lightCount > 0)
                _device.SetShaderParameter(parameter.handle, &_lightColors[0].x, _lightCount);
            break;

        case ShaderParameterSemantic::SkinningMatrices:
        {
            if (context.skinningMatrices == nullptr || context.boneCount == 0 || context.boneCount > MaxBones)
                return;

            dvl::Vec4 packedSkinningMatrices[MaxBones * 3];

            for (std::uint32_t boneIndex = 0; boneIndex < context.boneCount; boneIndex++)
            {
                for (int row = 0; row < 3; row++)
                {
                    packedSkinningMatrices[boneIndex * 3 + row] = dvl::Vec4(
                        context.skinningMatrices[boneIndex][0][row],
                        context.skinningMatrices[boneIndex][1][row],
                        context.skinningMatrices[boneIndex][2][row],
                        context.skinningMatrices[boneIndex][3][row]);
                }
            }

            _device.SetShaderParameter(parameter.handle, packedSkinningMatrices, context.boneCount * 3);
            break;
        }
    }
}
