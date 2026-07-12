#include <dvl/dvl.h>

#include <glm/glm.hpp>

#include <cstdint>

#include "engine/component/mesh_renderer.h"
#include "engine/core/transform.h"
#include "engine/core/world.h"
#include "engine/render/camera.h"
#include "engine/render/material.h"
#include "engine/render/mesh.h"
#include "engine/render/render_pipeline.h"
#include "engine/render/renderer.h"
#include "engine/render/vertex.h"

int main()
{
    dvl::Log(dvl::LogLevel::Info, "Application starting");

    constexpr int screenWidth = 960;
    constexpr int screenHeight = 544;

    Renderer renderer;
    if (!renderer.Initialize(screenWidth, screenHeight))
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to initialize renderer");
        return 1;
    }

    Mesh cubeMesh;
    Material solidMaterial;
    Material wireframeMaterial;
    World world;

    const VertexPositionColor vertices[] =
    {
        // Back
        {-0.5f, -0.5f, -0.5f, {0.0f, 0.0f, 0.0f, 1.0f}}, // 0
        { 0.5f, -0.5f, -0.5f, {1.0f, 0.0f, 0.0f, 1.0f}}, // 1
        {-0.5f,  0.5f, -0.5f, {0.0f, 1.0f, 0.0f, 1.0f}}, // 2
        { 0.5f,  0.5f, -0.5f, {1.0f, 1.0f, 0.0f, 1.0f}}, // 3

        // Front
        {-0.5f, -0.5f,  0.5f, {0.0f, 0.0f, 1.0f, 1.0f}}, // 4
        { 0.5f, -0.5f,  0.5f, {1.0f, 0.0f, 1.0f, 1.0f}}, // 5
        {-0.5f,  0.5f,  0.5f, {0.0f, 1.0f, 1.0f, 1.0f}}, // 6
        { 0.5f,  0.5f,  0.5f, {1.0f, 1.0f, 1.0f, 1.0f}}  // 7
    };

    const std::uint16_t indices[] =
    {
        // Front
        4, 5, 7,
        4, 7, 6,

        // Back
        1, 0, 2,
        1, 2, 3,

        // Left
        0, 4, 6,
        0, 6, 2,

        // Right
        5, 1, 3,
        5, 3, 7,

        // Top
        6, 7, 3,
        6, 3, 2,

        // Bottom
        0, 1, 5,
        0, 5, 4
    };

    MeshDesc meshDesc;
    meshDesc.vertexData = vertices;
    meshDesc.vertexDataSize = sizeof(vertices);
    meshDesc.indices = indices;
    meshDesc.indexCount = sizeof(indices) / sizeof(indices[0]);

    if (!renderer.CreateMesh(meshDesc, cubeMesh))
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to create cube mesh");
        renderer.Shutdown();
        return 1;
    }

    const dvl::VertexAttribute attributes[] =
    {
        {"aPosition", dvl::VertexFormat::Float3, offsetof(VertexPositionColor, x)},
        {"aColor", dvl::VertexFormat::Float4, offsetof(VertexPositionColor, color)}
    };

    RenderPipelineDesc pipelineDesc;
    pipelineDesc.vertexShaderPath = "app0:/assets/shaders/vertex.vert";
    pipelineDesc.fragmentShaderPath = "app0:/assets/shaders/fragment.frag";
    pipelineDesc.attributes = attributes;
    pipelineDesc.attributeCount = sizeof(attributes) / sizeof(attributes[0]);
    pipelineDesc.vertexStride = sizeof(VertexPositionColor);
    pipelineDesc.depthStencilState.depthTestEnabled = true;
    pipelineDesc.depthStencilState.depthWriteEnabled = true;

    solidMaterial.pipeline = renderer.CreateRenderPipeline(pipelineDesc);
    if (!solidMaterial.pipeline.IsValid())
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to create solid material pipeline");
        renderer.Shutdown();
        return 1;
    }
    
    pipelineDesc.rasterizerState.fillMode = dvl::FillMode::Wireframe;

    wireframeMaterial.pipeline = renderer.CreateRenderPipeline(pipelineDesc);
    if (!wireframeMaterial.pipeline.IsValid())
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to create wireframe material pipeline");
        renderer.Shutdown();
        return 1;
    }

    Camera camera(static_cast<float>(screenWidth), static_cast<float>(screenHeight));

    Entity* solidEntity = world.CreateEntity();
    solidEntity->transform.position = {1.0f, 0.0f, 0.0f};
    solidEntity->transform.rotation.x = glm::radians(35.264f);
    solidEntity->transform.rotation.z = glm::radians(45.0f);

    solidEntity->AddComponent<MeshRenderer>(&cubeMesh, &solidMaterial);

    Entity* wireframeEntity = world.CreateEntity();
    wireframeEntity->transform.position = {-1.0f, 0.0f, 0.0f};
    wireframeEntity->transform.rotation.x = glm::radians(35.264f);
    wireframeEntity->transform.rotation.z = glm::radians(45.0f);

    wireframeEntity->AddComponent<MeshRenderer>(&cubeMesh, &wireframeMaterial);

    float rotationAngle = 0.0f;

    while (true)
    {
        rotationAngle += 0.05f;

        if (Entity* entity = world.FindEntity(solidEntity->id))
            entity->transform.rotation.y = rotationAngle;

        if (Entity* entity = world.FindEntity(wireframeEntity->id))
            entity->transform.rotation.y = -rotationAngle;

        renderer.BeginFrame({0.118f, 0.122f, 0.278f});
        renderer.BeginScene(camera);

        for (const Entity* entity : world.GetEntities())
        {
            const MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();
            if (meshRenderer == nullptr || meshRenderer->mesh == nullptr ||  meshRenderer->material == nullptr)
            {
                continue;
            }

            renderer.Draw(*meshRenderer->mesh, *meshRenderer->material, entity->transform);
        }

        renderer.EndFrame();
    }

    world.GetEntities().clear();
    renderer.DestroyRenderPipeline(wireframeMaterial.pipeline);
    renderer.DestroyRenderPipeline(solidMaterial.pipeline);
    renderer.DestroyMesh(cubeMesh);
    renderer.Shutdown();

    return 0;
}
