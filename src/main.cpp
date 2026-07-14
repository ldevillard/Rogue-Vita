#include <dvl/dvl.h>

#include <glm/glm.hpp>

#include "engine/component/mesh_renderer.h"
#include "engine/core/asset_registry.h"
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

    World world = {};
    AssetRegistry assetRegistry = AssetRegistry(renderer);

    Camera camera(static_cast<float>(screenWidth), static_cast<float>(screenHeight));

    Entity* solidEntity = world.CreateEntity();
    solidEntity->transform.position = {1.0f, 0.0f, 0.0f};
    solidEntity->transform.rotation.x = glm::radians(35.264f);
    solidEntity->transform.rotation.z = glm::radians(45.0f);
    solidEntity->AddComponent<MeshRenderer>(&assetRegistry.GetCubeMesh(), &assetRegistry.GetSolidMaterial());

    Entity* wireframeEntity = world.CreateEntity();
    wireframeEntity->transform.position = {-1.0f, 0.0f, 0.0f};
    wireframeEntity->transform.rotation.x = glm::radians(35.264f);
    wireframeEntity->transform.rotation.z = glm::radians(45.0f);
    wireframeEntity->AddComponent<MeshRenderer>(&assetRegistry.GetCubeMesh(), &assetRegistry.GetWireframeMaterial());

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
