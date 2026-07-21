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

    MeshHandle practiceDummyMesh = assetRegistry.LoadMesh("app0:/asset/cooked/mesh/practice_dummy.dvlmesh", renderer);
    MeshHandle targetDummyMesh = assetRegistry.LoadMesh("app0:/asset/cooked/mesh/target_dummy.dvlmesh", renderer);
    MeshHandle trainingDummyMesh = assetRegistry.LoadMesh("app0:/asset/cooked/mesh/training_dummy.dvlmesh", renderer);

    Entity* cameraEntity = world.CreateEntity();
    Camera& mainCamera = cameraEntity->AddComponent<Camera>(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight), Camera::Orthographic);
    cameraEntity->transform.position = glm::vec3(-5.0f, 5.0f, -5.0f);
    cameraEntity->transform.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

    Entity* playerEntity = world.CreateEntity();
    playerEntity->transform.position = glm::vec3(0.75f, 1.0f, -0.75f);
    playerEntity->transform.scale = glm::vec3(2.5f);
    Material solidMaterial = assetRegistry.GetSolidMaterialInstance();
    solidMaterial.textureHandle = assetRegistry.LoadTexture("app0:/asset/cooked/texture/target_dummy.dvltex", renderer);

    playerEntity->AddComponent<MeshRenderer>(assetRegistry.GetMesh(targetDummyMesh), solidMaterial);
    playerEntity->AddComponent<PlayerController>(mainCamera);

    cameraEntity->AddComponent<SpringArm>(playerEntity->transform);

    Entity* targetEntity = world.CreateEntity();
    targetEntity->transform.position = glm::vec3(-0.75f, 1.3f, 0.75f);
    targetEntity->transform.scale = glm::vec3(2.5f);
    Material targetMaterial = assetRegistry.GetSolidMaterialInstance();
    targetMaterial.textureHandle = assetRegistry.LoadTexture("app0:/asset/cooked/texture/practice_dummy.dvltex", renderer);
    targetEntity->AddComponent<MeshRenderer>(assetRegistry.GetMesh(practiceDummyMesh), targetMaterial);

    Entity* trainingEntity = world.CreateEntity();
    trainingEntity->transform.position = glm::vec3(3.0f, 1.3f, 3.0f);
    trainingEntity->transform.rotation.y = glm::radians(-135.0f);
    trainingEntity->transform.scale = glm::vec3(2.5f);
    Material trainingMaterial = assetRegistry.GetSolidMaterialInstance();
    trainingMaterial.textureHandle = assetRegistry.LoadTexture("app0:/asset/cooked/texture/training_dummy.dvltex", renderer);
    trainingEntity->AddComponent<MeshRenderer>(assetRegistry.GetMesh(trainingDummyMesh), trainingMaterial);

    Entity* planeEntity = world.CreateEntity();
    planeEntity->transform.position = glm::vec3(0.0f);
    planeEntity->transform.scale = glm::vec3(8.0f, 0.1f, 8.0f);
    Material planeMaterial = assetRegistry.GetSolidMaterialInstance();
    planeMaterial.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    planeEntity->AddComponent<MeshRenderer>(&assetRegistry.GetCubeMesh(), planeMaterial);

    Entity* lightEntity = world.CreateEntity();
    DirectionalLight& light = lightEntity->AddComponent<DirectionalLight>();
    light.direction = glm::vec3(0.35f, -1.0f, 0.45f);
    light.intensity = 1.3f;

    float rotationAngle = 0.0f;

    while (true)
    {
        dvl::Time::Update();
        dvl::Input::Update();

        // Gameplay logic
        {
            const float deltaTime = dvl::Time::GetDeltaTime();

            // TODO: Rework entity traversal with RegisterComponent system in world to avoid
            // multiple traversal and casts
            for (const std::unique_ptr<Entity>& entity : world.GetEntities())
            {
                for (const std::unique_ptr<Component>& component : entity->GetComponents())
                {
                    if (Behavior* behavior = dynamic_cast<Behavior*>(component.get()))
                    {
                        behavior->Update(deltaTime);
                    }
                }
            }

            targetEntity->transform.rotation.y = rotationAngle;
            rotationAngle += 0.025f;

            // TODO: Create a dirty flag system in getters
            mainCamera.UpdateViewMatrix();
        }

        renderer.BeginFrame(glm::vec4(0.118f, 0.122f, 0.278f, 1.0f));
        renderer.BeginScene(mainCamera);

        // TODO: Use future World::GetLights
        for (const std::unique_ptr<Entity>& entity : world.GetEntities())
        {
            // With the future new register component system, it will support multiple lights per entity
            const DirectionalLight* directionalLight = entity->GetComponent<DirectionalLight>();
            if (directionalLight != nullptr)
                renderer.SubmitLight(*directionalLight);
        }

        // TODO: Use future World::GetMeshRenders
        for (const std::unique_ptr<Entity>& entity : world.GetEntities())
        {
            // With the future new register component system, it will support multiple mesh renderers per entity
            const MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();
            if (meshRenderer == nullptr)
                continue;

            const glm::mat4 modelMatrix = entity->transform.GetMatrix() * meshRenderer->localTransform.GetMatrix();
            renderer.Draw(*meshRenderer->mesh, meshRenderer->material, modelMatrix);
        }

        renderer.EndFrame();
    }

    assetRegistry.Shutdown(renderer);

    return 0;
}
