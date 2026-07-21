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
#include "game/component/spring_arm.h"

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
    solidEntity->transform.position = glm::vec3(0.75f, 0.0f, -0.75f);
    solidEntity->transform.scale = glm::vec3(2.5f);
    solidEntity->transform.rotation = glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f);
    Material solidMaterial = assetRegistry.GetSolidMaterialInstance();
    solidMaterial.textureHandle = assetRegistry.LoadTexture("app0:/asset/cooked/texture/practice_dummy.dvltex", renderer);;

    solidEntity->AddComponent<MeshRenderer>(assetRegistry.GetMesh(meshHandle), solidMaterial);
    PlayerController& playerController = solidEntity->AddComponent<PlayerController>(mainCamera);

    SpringArm& springArm = cameraEntity->AddComponent<SpringArm>(solidEntity->transform);

    Entity* wireframeEntity = world.CreateEntity();
    wireframeEntity->transform.position = glm::vec3(-0.75f, 0.0f, 0.75f);
    wireframeEntity->transform.scale = glm::vec3(2.5f);
    wireframeEntity->transform.rotation = solidEntity->transform.rotation;
    Material wireframeMaterial = assetRegistry.GetWireframeMaterialInstance();
    wireframeMaterial.textureHandle = solidMaterial.textureHandle;
    wireframeEntity->AddComponent<MeshRenderer>(assetRegistry.GetMesh(meshHandle), wireframeMaterial);

    Entity* planeEntity = world.CreateEntity();
    planeEntity->transform.position = glm::vec3(0.0f, -1.0f, 0.0f);
    planeEntity->transform.scale = glm::vec3(5.0f, 0.1f, 5.0f);
    Material planeMaterial = assetRegistry.GetSolidMaterialInstance();
    planeMaterial.color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
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
            springArm.Update(deltaTime);

            solidEntity->transform.rotation.z = rotationAngle;
            wireframeEntity->transform.rotation.z = rotationAngle;
            rotationAngle += 0.025f;

            // TODO: Check if camera need to be updated by the renderer or in the gameplay logic
            mainCamera.UpdateViewMatrix();
        }

        renderer.BeginFrame(glm::vec4(0.118f, 0.122f, 0.278f, 1.0f));
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
            if (meshRenderer == nullptr)
                continue;

            renderer.Draw(*meshRenderer->mesh, meshRenderer->material, entity->transform);
        }

        renderer.EndFrame();
    }

    assetRegistry.Shutdown(renderer);

    return 0;
}
