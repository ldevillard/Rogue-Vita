#include <dvl/dvl.h>

#include <glm/glm.hpp>

#include "engine/component/cube_mesh_renderer.h"
#include "engine/core/transform.h"
#include "engine/core/world.h"
#include "engine/render/camera.h"
#include "engine/render/material.h"
#include "engine/render/render_pipeline.h"
#include "engine/render/renderer.h"
#include "engine/render/vertex.h"

int main()
{
    dvl::Log(dvl::LogLevel::Info, "Application starting");

    constexpr int screenWidth = 960;
    constexpr int screenHeight = 544;

    Renderer renderer = Renderer(screenWidth, screenHeight);

    Material solidMaterial;
    Material wireframeMaterial;
    World world;

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
    pipelineDesc.rasterizerState.fillMode = dvl::FillMode::Wireframe;
    wireframeMaterial.pipeline = renderer.CreateRenderPipeline(pipelineDesc);

    Camera camera(static_cast<float>(screenWidth), static_cast<float>(screenHeight));

    Entity* solidEntity = world.CreateEntity();
    solidEntity->transform.position = {1.0f, 0.0f, 0.0f};
    solidEntity->transform.rotation.x = glm::radians(35.264f);
    solidEntity->transform.rotation.z = glm::radians(45.0f);
    solidEntity->AddComponent<CubeMeshRenderer>(renderer, &solidMaterial);

    Entity* wireframeEntity = world.CreateEntity();
    wireframeEntity->transform.position = {-1.0f, 0.0f, 0.0f};
    wireframeEntity->transform.rotation.x = glm::radians(35.264f);
    wireframeEntity->transform.rotation.z = glm::radians(45.0f);
    wireframeEntity->AddComponent<CubeMeshRenderer>(renderer, &wireframeMaterial);

    float rotationAngle = 0.0f;

    while (true)
    {
        rotationAngle += 0.05f;

        solidEntity->transform.rotation.y = rotationAngle;
        wireframeEntity->transform.rotation.y = -rotationAngle;

        renderer.BeginFrame({0.118f, 0.122f, 0.278f});
        renderer.BeginScene(camera);

        for (const std::unique_ptr<Entity>& entity : world.GetEntities())
        {
            const MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();
            if (meshRenderer == nullptr || !meshRenderer->IsValid())
                continue;

            renderer.Draw(*meshRenderer->mesh, *meshRenderer->material, entity->transform);
        }

        renderer.EndFrame();
    }

    return 0;
}
