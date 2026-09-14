#include <dvl/dvl.h>

#include <cmath>

#include "engine/component/behavior.h"
#include "engine/component/camera.h"
#include "engine/component/directional_light.h"
#include "engine/component/mesh_renderer.h"
#include "engine/component/skinned_mesh_renderer.h"
#include "engine/core/asset_registry.h"
#include "engine/core/world.h"
#include "engine/render/material.h"
#include "engine/render/renderer.h"

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

    const MeshHandle targetDummyMesh = assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/target_dummy.dvlmesh"), renderer);
    const SkeletonHandle targetDummySkeleton = assetRegistry.LoadSkeleton(dvl::Filesystem::GetAssetPath("cooked/skeleton/target_dummy.dvlskel"));
    const AnimationHandle runAnimationHandle = assetRegistry.LoadAnimation(dvl::Filesystem::GetAssetPath("cooked/animation/target_dummy@run_forward_in_place.dvlanim"));
    const MeshHandle pillarMesh = assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/ancient_pillar.dvlmesh"), renderer);

    const Skeleton* targetDummySkeletonAsset = assetRegistry.GetSkeleton(targetDummySkeleton);
    const Animation* runAnimationAsset = assetRegistry.GetAnimation(runAnimationHandle);
    const dvl::Skeleton targetSkeleton = targetDummySkeletonAsset->GetView();
    const dvl::Animation runAnimation = runAnimationAsset->GetView();

    std::vector<dvl::Transform> animatedPose(targetSkeleton.boneCount);
    std::vector<dvl::Mat4> skinningMatrices(targetSkeleton.boneCount);
    float runAnimationTime = 0.0f;

    Entity* cameraEntity = world.CreateEntity();
    Camera& mainCamera = cameraEntity->AddComponent<Camera>(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight), Camera::Orthographic);
    cameraEntity->transform.position = dvl::Vec3(-5.0f, 5.0f, -5.0f);
    cameraEntity->transform.LookAt(dvl::Vec3(0.0f, 0.0f, 0.0f));

    Entity* playerEntity = world.CreateEntity();
    playerEntity->transform.position = dvl::Vec3(0.75f, 1.0f, -0.75f);
    playerEntity->transform.scale = dvl::Vec3(2.5f, 2.5f, 2.5f);

    Material playerMaterial = assetRegistry.GetSolidMaterialInstance();
    playerMaterial.textureHandle = assetRegistry.LoadTexture(dvl::Filesystem::GetAssetPath("cooked/texture/target_dummy.dvltex"), renderer);
    playerEntity->AddComponent<SkinnedMeshRenderer>(assetRegistry.GetMesh(targetDummyMesh), playerMaterial, targetDummySkeletonAsset);
    playerEntity->AddComponent<PlayerController>(mainCamera);

    cameraEntity->AddComponent<SpringArm>(playerEntity->transform);

    Entity* pillarEntity = world.CreateEntity();
    pillarEntity->transform.position = dvl::Vec3(0.0f, 1.05f, 0.0f);
    pillarEntity->transform.scale = dvl::Vec3(3.0f, 3.0f, 3.0f);
    Material pillarMaterial = assetRegistry.GetSolidMaterialInstance();
    pillarMaterial.color = dvl::Vec4(0.6f, 0.6f, 0.6f, 1.0f);
    pillarEntity->AddComponent<MeshRenderer>(assetRegistry.GetMesh(pillarMesh), pillarMaterial);

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

        runAnimationTime = std::fmod(runAnimationTime + deltaTime, runAnimation.duration);
        dvl::Evaluate(runAnimation, runAnimationTime, animatedPose.data());
        dvl::LocalToWorld(targetSkeleton, animatedPose.data(), dvl::Mat4::Identity(), skinningMatrices.data());
        dvl::ComputeSkinningMatrices(targetSkeleton, skinningMatrices.data(), skinningMatrices.data());

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
                const dvl::Mat4 modelMatrix = entity->transform.GetMatrix() * skinnedMeshRenderer->localTransform.GetMatrix();
                renderer.DrawSkinned(*skinnedMeshRenderer->mesh, skinnedMeshRenderer->material, modelMatrix,
                    skinningMatrices.data(), targetSkeleton.boneCount);
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
