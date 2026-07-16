#include <dvl/dvl.h>

#include <glm/glm.hpp>

#include "engine/component/camera.h"
#include "engine/component/mesh_renderer.h"
#include "engine/component/directional_light.h"
#include "engine/core/asset_registry.h"
#include "engine/core/transform.h"
#include "engine/core/world.h"
#include "engine/render/material.h"
#include "engine/render/render_pipeline.h"
#include "engine/render/renderer.h"
#include "engine/render/vertex.h"

int main()
{
    dvl::Log(dvl::LogLevel::Info, "Application starting");

    constexpr int screenWidth = 960;
    constexpr int screenHeight = 544;

    AssetRegistry assetRegistry = {};
    Renderer renderer = Renderer(screenWidth, screenHeight, assetRegistry);

    assetRegistry.Initialize(renderer);

    World world = {};

    Entity* cameraEntity = world.CreateEntity();
    Camera& mainCamera = cameraEntity->AddComponent<Camera>(static_cast<float>(screenWidth), static_cast<float>(screenHeight), Camera::Orthographic);
    cameraEntity->transform.position = {-5.0f, 5.0f, -5.0f};
    cameraEntity->transform.LookAt({0.0f, 0.0f, 0.0f});

    Entity* solidEntity = world.CreateEntity();
    solidEntity->transform.position = {1.0f, 0.0f, 0.0f};
    solidEntity->transform.rotation.x = glm::radians(35.264f);
    solidEntity->transform.rotation.z = glm::radians(45.0f);
    Material redSolidMaterial = assetRegistry.GetSolidMaterialInstance();
    redSolidMaterial.color = { 1.0f, 0.0f, 0.0f, 1.0f };
    solidEntity->AddComponent<MeshRenderer>(&assetRegistry.GetCubeMesh(), redSolidMaterial);

    Entity* wireframeEntity = world.CreateEntity();
    wireframeEntity->transform.position = {-1.0f, 0.0f, 0.0f};
    wireframeEntity->transform.rotation.x = glm::radians(35.264f);
    wireframeEntity->transform.rotation.z = glm::radians(45.0f);
    wireframeEntity->AddComponent<MeshRenderer>(&assetRegistry.GetCubeMesh(), assetRegistry.GetWireframeMaterialInstance());

    Entity* planeEntity = world.CreateEntity();
    planeEntity->transform.position = {0.0f, -1.0f, 0.0f};
    planeEntity->transform.scale = {5.0f, 0.1f, 5.0f};
    Material planeMaterial = assetRegistry.GetSolidMaterialInstance();
    planeMaterial.color = {0.35f, 0.35f, 0.4f, 1.0f};
    planeEntity->AddComponent<MeshRenderer>(&assetRegistry.GetCubeMesh(), planeMaterial);

    Entity* lightEntity = world.CreateEntity();
    DirectionalLight& light = lightEntity->AddComponent<DirectionalLight>();
    light.direction = glm::normalize(glm::vec3{0.0f, -1.0f, 0.0f});
    light.intensity = 1;

    float rotationAngle = 0.0f;

    while (true)
    {
        rotationAngle += 0.025f;

        solidEntity->transform.rotation.y = rotationAngle;
        wireframeEntity->transform.rotation.y = -rotationAngle;

        renderer.BeginFrame({0.118f, 0.122f, 0.278f});
        renderer.BeginScene(mainCamera);

        for (const std::unique_ptr<Entity>& entity : world.GetEntities())
        {
            const DirectionalLight* directionalLight = entity->GetComponent<DirectionalLight>();
            if (directionalLight != nullptr)
                renderer.SubmitLight(*directionalLight);
        }

        for (const std::unique_ptr<Entity>& entity : world.GetEntities())
        {
            const MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();
            if (meshRenderer == nullptr || !meshRenderer->IsValid())
                continue;

            renderer.Draw(*meshRenderer->mesh, meshRenderer->material, entity->transform);
        }

        renderer.EndFrame();
    }

    assetRegistry.Shutdown(renderer);

    return 0;
}
