#include <dvl/dvl.h>

#include "engine/component/animator.h"
#include "engine/component/behavior.h"
#include "engine/component/box_collider.h"
#include "engine/component/camera.h"
#include "engine/component/directional_light.h"
#include "engine/component/mesh_renderer.h"
#include "engine/component/skinned_mesh_renderer.h"
#include "engine/core/asset_registry.h"
#include "engine/core/entity.h"
#include "engine/core/world.h"
#include "engine/debug/debug_draw.h"
#include "engine/physics/physics.h"
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
    DebugDraw::Initialize(assetRegistry, &renderer);

    dvl::Input::Initialize();
    dvl::Time::Initialize();

    World world = {};
    AnimationSystem animationSystem = {};

    Physics::Initialize(&world);

    const MeshHandle playerMesh = assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/target_dummy.dvlmesh"), renderer);
    const SkeletonHandle playerSkeleton = assetRegistry.LoadSkeleton(dvl::Filesystem::GetAssetPath("cooked/skeleton/target_dummy.dvlskel"));
    const AnimationHandle playerIdleAnimation = assetRegistry.LoadAnimation(dvl::Filesystem::GetAssetPath("cooked/animation/target_dummy@idle.dvlanim"));
    const AnimationHandle playerRunAnimation = assetRegistry.LoadAnimation(dvl::Filesystem::GetAssetPath("cooked/animation/target_dummy@run_forward_in_place.dvlanim"));
    const AnimationHandle playerAttackAnimation = assetRegistry.LoadAnimation(dvl::Filesystem::GetAssetPath("cooked/animation/target_dummy@right_projectile_attack.dvlanim"));

    const MeshHandle practiceDummyMesh = assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/practice_dummy.dvlmesh"), renderer);
    const SkeletonHandle practiceDummySkeleton = assetRegistry.LoadSkeleton(dvl::Filesystem::GetAssetPath("cooked/skeleton/practice_dummy.dvlskel"));
    const AnimationHandle practiceDummyIdleAnimation = assetRegistry.LoadAnimation(dvl::Filesystem::GetAssetPath("cooked/animation/practice_dummy@idle.dvlanim"));

    Entity* cameraEntity = world.CreateEntity();
    Camera& mainCamera = cameraEntity->AddComponent<Camera>(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight), Camera::Orthographic);
    cameraEntity->transform.position = dvl::Vec3(-5.0f, 5.0f, -5.0f);
    cameraEntity->transform.LookAt(dvl::Vec3::Zero());

    Entity* playerEntity = world.CreateEntity();
    playerEntity->transform.position = dvl::Vec3(0.75f, 0.5f, -0.75f);
    playerEntity->transform.scale = dvl::Vec3::One();

    Material playerMaterial = assetRegistry.GetSolidMaterialInstance();
    playerMaterial.textureHandle = assetRegistry.LoadTexture(dvl::Filesystem::GetAssetPath("cooked/texture/target_dummy.dvltex"), renderer);
    playerEntity->AddComponent<SkinnedMeshRenderer>(playerMesh, playerMaterial, playerSkeleton);
    Animator& playerAnimator = playerEntity->AddComponent<Animator>(playerSkeleton, playerIdleAnimation);
    const PlayerAnimation playerAnimations =
    {
        {playerIdleAnimation, AnimationLoopMode::Loop, 1.0f},
        {playerRunAnimation, AnimationLoopMode::Loop, 1.0f},
        {playerAttackAnimation, AnimationLoopMode::Once, 1.5f}
    };
    playerEntity->AddComponent<PlayerController>(cameraEntity->ToRef(), playerAnimator.ToRef<Animator>(), playerAnimations);
    playerEntity->AddComponent<BoxCollider>(dvl::Vec3::Zero(), dvl::Vec3(0.5f, 0.8f, 0.5f));

    cameraEntity->AddComponent<SpringArm>(playerEntity->ToRef());

    Entity* practiceDummyEntity = world.CreateEntity();
    practiceDummyEntity->transform.position = dvl::Vec3(0.0f, 0.5f, 0.0f);
    practiceDummyEntity->transform.scale = dvl::Vec3::One();
    practiceDummyEntity->AddComponent<BoxCollider>(dvl::Vec3(0.0f, 0.25f, 0.0f), dvl::Vec3(0.5f, 0.8f, 0.5f));

    dvl::Vec3 practiceDummyCameraDirection = cameraEntity->transform.position - practiceDummyEntity->transform.position;
    practiceDummyCameraDirection.y = 0.0f;
    practiceDummyEntity->transform.LookDirection(practiceDummyCameraDirection);

    Material practiceDummyMaterial = assetRegistry.GetSolidMaterialInstance();
    practiceDummyMaterial.textureHandle = assetRegistry.LoadTexture(dvl::Filesystem::GetAssetPath("cooked/texture/practice_dummy.dvltex"), renderer);
    practiceDummyEntity->AddComponent<SkinnedMeshRenderer>(practiceDummyMesh, practiceDummyMaterial, practiceDummySkeleton);
    practiceDummyEntity->AddComponent<Animator>(practiceDummySkeleton, practiceDummyIdleAnimation);

    Entity* planeEntity = world.CreateEntity();
    planeEntity->transform.position = dvl::Vec3(0.0f, 0.0f, 0.0f);
    planeEntity->transform.scale = dvl::Vec3(6.0f, 0.1f, 6.0f);
    Material planeMaterial = assetRegistry.GetSolidMaterialInstance();
    planeMaterial.color = dvl::Vec4(0.4f, 0.4f, 0.4f, 1.0f);
    planeEntity->AddComponent<MeshRenderer>(assetRegistry.GetCubeMeshHandle(), planeMaterial);

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
        world.StartPendingComponents();

        renderer.BeginFrame(dvl::Vec4(0.32f, 0.45f, 0.65f, 1.0f));
        renderer.BeginScene(mainCamera);

        // Gameplay logic
        {
            for (Behavior* behavior : world.GetComponents<Behavior>())
            {
                behavior->Update(deltaTime);
            }

            // Initialize components instantiated during gameplay before rendering
            world.StartPendingComponents();
            mainCamera.UpdateViewMatrix();
        }
        
        animationSystem.Update(world, assetRegistry, deltaTime);

        for (DirectionalLight* directionalLight : world.GetComponents<DirectionalLight>())
        {
            renderer.SubmitLight(*directionalLight);
        }

        // Render logic
        for (const MeshRenderer* meshRenderer : world.GetComponents<MeshRenderer>())
        {
            const Mesh* mesh = assetRegistry.GetMesh(meshRenderer->meshHandle);
            if (mesh == nullptr)
                continue;

            const Entity* entity = meshRenderer->GetEntity();

            const SkinnedMeshRenderer* skinnedMeshRenderer = dynamic_cast<const SkinnedMeshRenderer*>(meshRenderer);

            if (skinnedMeshRenderer != nullptr)
            {
                const Animator* animator = entity->GetComponent<Animator>();
                if (animator == nullptr || !animator->IsValid())
                    continue;

                const dvl::Mat4 modelMatrix = entity->transform.GetMatrix() * skinnedMeshRenderer->localTransform.GetMatrix();
                renderer.DrawSkinned(*mesh, skinnedMeshRenderer->material, modelMatrix, animator->GetSkinningMatrices(), animator->GetBoneCount());
                
                continue;
            }

            const dvl::Mat4 modelMatrix = entity->transform.GetMatrix() * meshRenderer->localTransform.GetMatrix();
            renderer.Draw(*mesh, meshRenderer->material, modelMatrix);
        }

        // Debug editor only logic
        for (BoxCollider* boxCollider : world.GetComponents<BoxCollider>())
        {
            const Transform& transform = boxCollider->GetTransform();
            DebugDraw::DrawWireCube(boxCollider->box.center, boxCollider->box.size, dvl::Vec4(0.0f, 1.0f, 0.0f, 1.0f), transform.GetMatrix());
        }

        renderer.EndFrame();
    }

    assetRegistry.Shutdown(renderer);
    return 0;
}
