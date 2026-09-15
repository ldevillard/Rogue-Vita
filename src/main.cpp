#include <dvl/dvl.h>

#include "engine/component/animator.h"
#include "engine/component/behavior.h"
#include "engine/component/camera.h"
#include "engine/component/directional_light.h"
#include "engine/component/mesh_renderer.h"
#include "engine/component/skinned_mesh_renderer.h"
#include "engine/core/asset_registry.h"
#include "engine/core/world.h"
#include "engine/render/material.h"
#include "engine/render/renderer.h"
#include "engine/system/animation_system.h"

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
    AnimationSystem animationSystem = {};

    const MeshHandle targetDummyMesh = assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/target_dummy.dvlmesh"), renderer);
    const SkeletonHandle targetDummySkeleton = assetRegistry.LoadSkeleton(dvl::Filesystem::GetAssetPath("cooked/skeleton/target_dummy.dvlskel"));
    const AnimationHandle runAnimationHandle = assetRegistry.LoadAnimation(dvl::Filesystem::GetAssetPath("cooked/animation/target_dummy@walk_forward_in_place.dvlanim"));
    const MeshHandle trainingDummyMesh = assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/training_dummy.dvlmesh"), renderer);
    const SkeletonHandle trainingDummySkeleton = assetRegistry.LoadSkeleton(dvl::Filesystem::GetAssetPath("cooked/skeleton/training_dummy.dvlskel"));
    const AnimationHandle idleAnimationHandle = assetRegistry.LoadAnimation(dvl::Filesystem::GetAssetPath("cooked/animation/training_dummy@idle.dvlanim"));
    const MeshHandle practiceDummyMesh = assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/practice_dummy.dvlmesh"), renderer);
    const SkeletonHandle practiceDummySkeleton = assetRegistry.LoadSkeleton(dvl::Filesystem::GetAssetPath("cooked/skeleton/practice_dummy.dvlskel"));
    const AnimationHandle practiceIdleAnimationHandle = assetRegistry.LoadAnimation(dvl::Filesystem::GetAssetPath("cooked/animation/practice_dummy@idle.dvlanim"));
    const MeshHandle dragonInfernoMesh = assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/dragon_inferno.dvlmesh"), renderer);
    const SkeletonHandle dragonInfernoSkeleton = assetRegistry.LoadSkeleton(dvl::Filesystem::GetAssetPath("cooked/skeleton/dragon_inferno.dvlskel"));
    const AnimationHandle dragonFlyIdleAnimation = assetRegistry.LoadAnimation(dvl::Filesystem::GetAssetPath("cooked/animation/dragon_inferno@fly_idle.dvlanim"));
    //const MeshHandle pillarMesh = assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/ancient_pillar.dvlmesh"), renderer);

    const Skeleton* targetDummySkeletonAsset = assetRegistry.GetSkeleton(targetDummySkeleton);
    const Skeleton* trainingDummySkeletonAsset = assetRegistry.GetSkeleton(trainingDummySkeleton);
    const Skeleton* practiceDummySkeletonAsset = assetRegistry.GetSkeleton(practiceDummySkeleton);
    const Skeleton* dragonInfernoSkeletonAsset = assetRegistry.GetSkeleton(dragonInfernoSkeleton);

    Entity* cameraEntity = world.CreateEntity();
    Camera& mainCamera = cameraEntity->AddComponent<Camera>(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight), Camera::Orthographic);
    cameraEntity->transform.position = dvl::Vec3(-5.0f, 5.0f, -5.0f);
    cameraEntity->transform.LookAt(dvl::Vec3(0.0f, 0.0f, 0.0f));

    Entity* playerEntity = world.CreateEntity();
    playerEntity->transform.position = dvl::Vec3(0.75f, 1.0f, -0.75f);
    playerEntity->transform.scale = dvl::Vec3(2.0f, 2.0f, 2.0f);

    Material playerMaterial = assetRegistry.GetSolidMaterialInstance();
    playerMaterial.textureHandle = assetRegistry.LoadTexture(dvl::Filesystem::GetAssetPath("cooked/texture/target_dummy.dvltex"), renderer);
    playerEntity->AddComponent<SkinnedMeshRenderer>(assetRegistry.GetMesh(targetDummyMesh), playerMaterial, targetDummySkeletonAsset);
    playerEntity->AddComponent<Animator>(targetDummySkeleton, runAnimationHandle);
    playerEntity->AddComponent<PlayerController>(mainCamera);

    Entity* trainingDummyEntity = world.CreateEntity();
    trainingDummyEntity->transform.position = dvl::Vec3(-3.0f, 0.0f, 0.75f);
    trainingDummyEntity->transform.scale = dvl::Vec3(2.5f, 2.5f, 2.5f);

    Material trainingDummyMaterial = assetRegistry.GetSolidMaterialInstance();
    trainingDummyMaterial.textureHandle = assetRegistry.LoadTexture(dvl::Filesystem::GetAssetPath("cooked/texture/training_dummy.dvltex"), renderer);
    SkinnedMeshRenderer& trainingDummyRenderer = trainingDummyEntity->AddComponent<SkinnedMeshRenderer>(assetRegistry.GetMesh(trainingDummyMesh), trainingDummyMaterial, trainingDummySkeletonAsset);
    trainingDummyRenderer.localTransform.rotation.y = -dvl::Pi * 0.5f;
    trainingDummyRenderer.localTransform.rotation.z = dvl::Pi;
    trainingDummyEntity->AddComponent<Animator>(trainingDummySkeleton, idleAnimationHandle);

    Entity* practiceDummyEntity = world.CreateEntity();
    practiceDummyEntity->transform.position = dvl::Vec3(2.5f, 0.5f, 0.75f);
    practiceDummyEntity->transform.scale = dvl::Vec3(2.0f, 2.0f, 2.0f);

    Material practiceDummyMaterial = assetRegistry.GetSolidMaterialInstance();
    practiceDummyMaterial.textureHandle = assetRegistry.LoadTexture(dvl::Filesystem::GetAssetPath("cooked/texture/practice_dummy.dvltex"), renderer);
    practiceDummyEntity->AddComponent<SkinnedMeshRenderer>(assetRegistry.GetMesh(practiceDummyMesh), practiceDummyMaterial, practiceDummySkeletonAsset);
    practiceDummyEntity->AddComponent<Animator>(practiceDummySkeleton, practiceIdleAnimationHandle);

    Entity* dragonInfernoEntity = world.CreateEntity();
    dragonInfernoEntity->transform.position = dvl::Vec3(0.0f, 1.0f, 2.5f);
    dragonInfernoEntity->transform.scale = dvl::Vec3(4.0f, 4.0f, 4.0f);

    Material dragonInfernoMaterial = assetRegistry.GetSolidMaterialInstance();
    dragonInfernoMaterial.textureHandle = assetRegistry.LoadTexture(dvl::Filesystem::GetAssetPath("cooked/texture/dragon_inferno.dvltex"), renderer);
    dragonInfernoEntity->AddComponent<SkinnedMeshRenderer>(assetRegistry.GetMesh(dragonInfernoMesh), dragonInfernoMaterial, dragonInfernoSkeletonAsset);
    dragonInfernoEntity->AddComponent<Animator>(dragonInfernoSkeleton, dragonFlyIdleAnimation);

    cameraEntity->AddComponent<SpringArm>(playerEntity->transform);

    //Entity* pillarEntity = world.CreateEntity();
    //pillarEntity->transform.position = dvl::Vec3(0.0f, 1.05f, 0.0f);
    //pillarEntity->transform.scale = dvl::Vec3(3.0f, 3.0f, 3.0f);
    //Material pillarMaterial = assetRegistry.GetSolidMaterialInstance();
    //pillarMaterial.color = dvl::Vec4(0.6f, 0.6f, 0.6f, 1.0f);
    //pillarEntity->AddComponent<MeshRenderer>(assetRegistry.GetMesh(pillarMesh), pillarMaterial);

    Entity* planeEntity = world.CreateEntity();
    planeEntity->transform.position = dvl::Vec3(0.0f, 0.0f, 0.0f);
    planeEntity->transform.scale = dvl::Vec3(8.0f, 0.1f, 8.0f);
    Material planeMaterial = assetRegistry.GetSolidMaterialInstance();
    planeMaterial.color = dvl::Vec4(0.4f, 0.4f, 0.4f, 1.0f);
    planeEntity->AddComponent<MeshRenderer>(&assetRegistry.GetCubeMesh(), planeMaterial);

    Entity* lightEntity = world.CreateEntity();
    DirectionalLight& light = lightEntity->AddComponent<DirectionalLight>();
    light.direction = dvl::Vec3(0.35f, -1.0f, 0.45f);
    light.intensity = 1.3f;

    while (renderer.ShouldClose() == false)
    {
        dvl::Time::Update();
        dvl::Input::Update();

        const float deltaTime = dvl::Time::GetDeltaTime();

        dvl::Tweener::Update(deltaTime);

        // Gameplay logic
        {
            for (const std::unique_ptr<Entity>& entity : world.GetEntities())
            {
                for (const std::unique_ptr<Component>& component : entity->GetComponents())
                {
                    if (Behavior* behavior = dynamic_cast<Behavior*>(component.get()))
                        behavior->Update(deltaTime);
                }
            }

            mainCamera.UpdateViewMatrix();
        }

        animationSystem.Update(world, assetRegistry, deltaTime);

        renderer.BeginFrame(dvl::Vec4(0.32f, 0.45f, 0.65f, 1.0f));
        renderer.BeginScene(mainCamera);

        for (const std::unique_ptr<Entity>& entity : world.GetEntities())
        {
            const DirectionalLight* directionalLight = entity->GetComponent<DirectionalLight>();
            if (directionalLight != nullptr)
                renderer.SubmitLight(*directionalLight);
        }

        for (const std::unique_ptr<Entity>& entity : world.GetEntities())
        {
            const SkinnedMeshRenderer* skinnedMeshRenderer = entity->GetComponent<SkinnedMeshRenderer>();
            if (skinnedMeshRenderer != nullptr)
            {
                const Animator* animator = entity->GetComponent<Animator>();
                if (animator == nullptr || !animator->IsValid())
                    continue;

                const dvl::Mat4 modelMatrix = entity->transform.GetMatrix() * skinnedMeshRenderer->localTransform.GetMatrix();
                renderer.DrawSkinned(*skinnedMeshRenderer->mesh, skinnedMeshRenderer->material, modelMatrix, animator->GetSkinningMatrices(), animator->GetBoneCount());
                
                continue;
            }

            const MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();
            if (meshRenderer != nullptr)
            {
                const dvl::Mat4 modelMatrix = entity->transform.GetMatrix() * meshRenderer->localTransform.GetMatrix();
                renderer.Draw(*meshRenderer->mesh, meshRenderer->material, modelMatrix);
            }
        }

        renderer.EndFrame();
    }

    assetRegistry.Shutdown(renderer);
    return 0;
}
