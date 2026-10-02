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

#include "game/component/enemy.h"
#include "game/component/player_controller.h"
#include "game/component/projectile.h"
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
    const AnimationHandle practiceDummyTakeDamageAnimation = assetRegistry.LoadAnimation(dvl::Filesystem::GetAssetPath("cooked/animation/practice_dummy@take_damage.dvlanim"));

    Entity* cameraEntity = world.CreateEntity();
    Camera& mainCamera = cameraEntity->AddComponent<Camera>(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight), Camera::Orthographic);
    cameraEntity->transform.position = dvl::Vec3(-5.0f, 5.0f, -5.0f);
    cameraEntity->transform.LookAt(dvl::Vec3::Zero());

    Entity* playerEntity = world.CreateEntity();
    playerEntity->transform.position = dvl::Vec3(0.75f, 0.5f, -0.75f);
    playerEntity->transform.scale = dvl::Vec3::One();

    Entity* bulletPrefab = world.CreateEntity();
    Material bulletMaterial = assetRegistry.GetSolidMaterialInstance();
    bulletMaterial.color = dvl::Vec4(1.0f, 0.6f, 0.0f, 1.0f);
    bulletPrefab->AddComponent<MeshRenderer>(assetRegistry.GetSphereMeshHandle(), bulletMaterial);
    bulletPrefab->transform.position = dvl::Vec3(2.0f, 1.0f, 2.0f);
    bulletPrefab->transform.scale = dvl::Vec3(0.15f, 0.15f, 0.15f);
    bulletPrefab->AddComponent<Projectile>();

    Material playerMaterial = assetRegistry.GetSolidMaterialInstance();
    playerMaterial.textureHandle = assetRegistry.LoadTexture(dvl::Filesystem::GetAssetPath("cooked/texture/target_dummy.dvltex"), renderer);
    playerEntity->AddComponent<SkinnedMeshRenderer>(playerMesh, playerMaterial, playerSkeleton);
    Animator& playerAnimator = playerEntity->AddComponent<Animator>(playerSkeleton, playerIdleAnimation);
    const CharacterAnimations playerAnimations =
    {
        {playerIdleAnimation, AnimationLoopMode::Loop, 1.0f},
        {playerRunAnimation, AnimationLoopMode::Loop, 1.0f},
        {playerAttackAnimation, AnimationLoopMode::Once, 3.0f},
        { /* TODO: Add take damage animation*/ }
    };
    playerEntity->AddComponent<PlayerController>(cameraEntity->ToRef(), playerAnimator.ToRef<Animator>(), playerAnimations, bulletPrefab->ToRef());
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
    CharacterAnimations practiceDummyAnimations = {};
    practiceDummyAnimations.idle = {practiceDummyIdleAnimation, AnimationLoopMode::Loop, 1.0f};
    practiceDummyAnimations.takeDamage = {practiceDummyTakeDamageAnimation, AnimationLoopMode::Once, 1.0f};

    practiceDummyEntity->AddComponent<Enemy>(practiceDummyAnimations);

    Material environmentMaterial = assetRegistry.GetSolidMaterialInstance();
    environmentMaterial.textureHandle = assetRegistry.LoadTexture(dvl::Filesystem::GetAssetPath("cooked/texture/small_scene_forest_ruins.dvltex"), renderer);

    Material groundMaterial = environmentMaterial;
    groundMaterial.color = dvl::Vec4(0.8f, 0.8f, 0.8f, 1.0f);
    Entity* groundEntity = world.CreateEntity();
    groundEntity->transform.scale = dvl::Vec3(6.0f, 0.5f, 6.0f);
    groundEntity->transform.rotation = dvl::Quat(0.0f, 1.0f, 0.0f, 0.0f);
    groundEntity->AddComponent<MeshRenderer>(assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/small_scene_battleground_01.dvlmesh"), renderer), groundMaterial);

    const MeshHandle wallEndMesh = assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/border_end_02.dvlmesh"), renderer);
    const MeshHandle wallMiddleMesh = assetRegistry.LoadMesh(dvl::Filesystem::GetAssetPath("cooked/mesh/border_middle_02.dvlmesh"), renderer);

    Entity* wallEntity = world.CreateEntity();
    wallEntity->transform.position = dvl::Vec3(2.3f, 0.05f, 0.0f);
    wallEntity->transform.rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 1.0f, 0.0f), dvl::Radians(90.0f));
    wallEntity->transform.scale = dvl::Vec3(0.8f, 0.8f, 0.8f);

    MeshRenderer& wallLeftPillar = wallEntity->AddComponent<MeshRenderer>(wallEndMesh, environmentMaterial);
    wallLeftPillar.localTransform.position = dvl::Vec3(-1.7f, 0.7f, 0.0f);
    wallLeftPillar.localTransform.rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 1.0f, 0.0f), dvl::Radians(270.0f));
    wallLeftPillar.localTransform.scale = dvl::Vec3(1.4f, 1.4f, 1.4f);

    MeshRenderer& wallLeftSection = wallEntity->AddComponent<MeshRenderer>(wallMiddleMesh, environmentMaterial);
    wallLeftSection.localTransform.position = dvl::Vec3(-0.85f, 0.55f, 0.0f);
    wallLeftSection.localTransform.rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 1.0f, 0.0f), dvl::Radians(180.0f));
    wallLeftSection.localTransform.scale = dvl::Vec3(1.4f, 1.3f, 1.3f);

    MeshRenderer& wallCenterPillar = wallEntity->AddComponent<MeshRenderer>(wallEndMesh, environmentMaterial);
    wallCenterPillar.localTransform.position = dvl::Vec3(0.0f, 0.7f, 0.0f);
    wallCenterPillar.localTransform.rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 1.0f, 0.0f), dvl::Radians(270.0f));
    wallCenterPillar.localTransform.scale = dvl::Vec3(1.4f, 1.4f, 1.4f);

    MeshRenderer& wallRightSection = wallEntity->AddComponent<MeshRenderer>(wallMiddleMesh, environmentMaterial);
    wallRightSection.localTransform.position = dvl::Vec3(0.85f, 0.55f, 0.0f);
    wallRightSection.localTransform.rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 1.0f, 0.0f), dvl::Radians(180.0f));
    wallRightSection.localTransform.scale = dvl::Vec3(1.4f, 1.3f, 1.3f);

    MeshRenderer& wallRightPillar = wallEntity->AddComponent<MeshRenderer>(wallEndMesh, environmentMaterial);
    wallRightPillar.localTransform.position = dvl::Vec3(1.7f, 0.7f, 0.0f);
    wallRightPillar.localTransform.rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 1.0f, 0.0f), dvl::Radians(270.0f));
    wallRightPillar.localTransform.scale = dvl::Vec3(1.4f, 1.4f, 1.4f);

    wallEntity->AddComponent<BoxCollider>(dvl::Vec3(0.0f, 0.7f, 0.0f), dvl::Vec3(4.0f, 1.4f, 0.6f));

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
