#include <dvl/dvl.h>

#include <glm/glm.hpp>

#include "engine/component/camera.h"
#include "engine/component/directional_light.h"
#include "engine/component/mesh_renderer.h"
#include "engine/core/asset_registry.h"
#include "engine/core/transform.h"
#include "engine/core/world.h"
#include "engine/render/material.h"
#include "engine/render/render_pipeline.h"
#include "engine/render/renderer.h"
#include "engine/render/vertex.h"

#include "game/component/player_controller.h"

int main()
{
    dvl::Log(dvl::LogLevel::Info, "Application starting");

    constexpr int ScreenWidth = 960;
    constexpr int ScreenHeight = 544;

    AssetRegistry assetRegistry = {};
    Renderer renderer = Renderer(ScreenWidth, ScreenHeight, assetRegistry);

    assetRegistry.Initialize(renderer);

    dvl::Input::Initialize();
    dvl::Time::Initialize();

    World world = {};

    MeshHandle meshHandle = assetRegistry.LoadMesh("app0:/asset/cooked/mesh/practice_dummy.dvlmesh", renderer);

    Entity* cameraEntity = world.CreateEntity();
    Camera& mainCamera = cameraEntity->AddComponent<Camera>(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight), Camera::Orthographic);
    cameraEntity->transform.position = glm::vec3(-5.0f, 5.0f, -5.0f);
    cameraEntity->transform.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

    Entity* solidEntity = world.CreateEntity();
    solidEntity->transform.scale = glm::vec3(3);
    solidEntity->transform.rotation = { glm::radians(-90.0f), glm::radians(0.0f), glm::radians(-130.0f) };
    Material redSolidMaterial = assetRegistry.GetSolidMaterialInstance();
    redSolidMaterial.color = { 1.0f, 1.0f, 1.0f, 1.0f };
    const dvl::TextureHandle texture = assetRegistry.LoadTexture("app0:/asset/cooked/texture/practice_dummy.dvltex", renderer);
    redSolidMaterial.textureHandle = texture;

    solidEntity->AddComponent<MeshRenderer>(assetRegistry.GetMesh(meshHandle), redSolidMaterial);
    PlayerController& playerController = solidEntity->AddComponent<PlayerController>(mainCamera);

    Entity* wireframeEntity = world.CreateEntity();
    wireframeEntity->transform.position = glm::vec3(-1.0f, 0.0f, 0.0f);
    wireframeEntity->transform.rotation.x = glm::radians(35.264f);
    wireframeEntity->transform.rotation.z = glm::radians(45.0f);
    wireframeEntity->AddComponent<MeshRenderer>(&assetRegistry.GetCubeMesh(), assetRegistry.GetWireframeMaterialInstance());

    Entity* planeEntity = world.CreateEntity();
    planeEntity->transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
    planeEntity->transform.scale = glm::vec3(5.0f, 0.1f, 5.0f);
    Material planeMaterial = assetRegistry.GetSolidMaterialInstance();
    planeMaterial.color = {0.35f, 0.35f, 0.4f, 1.0f};
    planeEntity->AddComponent<MeshRenderer>(&assetRegistry.GetCubeMesh(), planeMaterial);

    Entity* lightEntity = world.CreateEntity();
    DirectionalLight& light = lightEntity->AddComponent<DirectionalLight>();
    light.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    light.intensity = 1.5f;

    float rotationAngle = 0.0f;

    while (true)
    {
        // Gameplay logic
        {
            dvl::Time::Update();
            dvl::Input::Update();

            const float deltaTime = dvl::Time::GetDeltaTime();
            playerController.Update(deltaTime);

            rotationAngle += 0.025f;
            wireframeEntity->transform.rotation.y = -rotationAngle;

            // TODO: Check if camera need to be updated by the renderer or in the gameplay logic
            mainCamera.UpdateViewMatrix();
        }

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
