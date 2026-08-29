#include <dvl/dvl.h>

#include <cstddef>
#include <cstdint>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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

namespace
{
    constexpr int BoneCount = 2;
    constexpr std::size_t MaxBoneInfluences = 4;
    constexpr std::size_t AnimatedVertexCount = 6;

    struct CpuSkinVertex
    {
        VertexPositionNormalUV bindVertex;
        std::uint8_t boneIndices[MaxBoneInfluences];
        float boneWeights[MaxBoneInfluences];
    };

    void SkinVerticesCpu(const CpuSkinVertex* bindVertices, std::size_t vertexCount, const dvl::Mat4* skinningMatrices, VertexPositionNormalUV* skinnedVertices)
    {
        for (std::size_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
        {
            const VertexPositionNormalUV& bindVertex = bindVertices[vertexIndex].bindVertex;
            const dvl::Vec4 bindPosition(bindVertex.x, bindVertex.y, bindVertex.z, 1.0f);
            const dvl::Vec4 bindNormal(bindVertex.nx, bindVertex.ny, bindVertex.nz, 0.0f);

            dvl::Vec4 position(0.0f, 0.0f, 0.0f, 0.0f);
            dvl::Vec4 normal(0.0f, 0.0f, 0.0f, 0.0f);

            for (std::size_t influenceIndex = 0; influenceIndex < MaxBoneInfluences; influenceIndex++)
            {
                const float boneWeight = bindVertices[vertexIndex].boneWeights[influenceIndex];

                if (boneWeight <= 0.0f)
                    continue;

                const std::uint8_t boneIndex = bindVertices[vertexIndex].boneIndices[influenceIndex];
                position += skinningMatrices[boneIndex] * bindPosition * boneWeight;
                normal += skinningMatrices[boneIndex] * bindNormal * boneWeight;
            }

            skinnedVertices[vertexIndex].x = position.x;
            skinnedVertices[vertexIndex].y = position.y;
            skinnedVertices[vertexIndex].z = position.z;

            normal.Normalize();
            skinnedVertices[vertexIndex].nx = normal.x;
            skinnedVertices[vertexIndex].ny = normal.y;
            skinnedVertices[vertexIndex].nz = normal.z;
        }
    }
}

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

    MeshHandle targetDummyMesh = assetRegistry.LoadMesh("app0:/asset/cooked/mesh/target_dummy.dvlmesh", renderer);

    /*
    MeshHandle practiceDummyMesh = assetRegistry.LoadMesh("app0:/asset/cooked/mesh/practice_dummy.dvlmesh", renderer);
    MeshHandle trainingDummyMesh = assetRegistry.LoadMesh("app0:/asset/cooked/mesh/training_dummy.dvlmesh", renderer);
    */

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

    /*
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

    constexpr float JumpHeight = 1.0f;
    constexpr float JumpDuration = 1.0f;
    const float trainingStartHeight = trainingEntity->transform.position.y;

    // TODO: Replace this sine-based loop with a yoyo option in the tween system
    dvl::Tweener::Create(0.0f, glm::pi<float>(), JumpDuration, dvl::Easing::Linear, -1)
        .OnUpdate([trainingEntity, trainingStartHeight](const float& jumpTime)
        {
            trainingEntity->transform.position.y = trainingStartHeight + std::sin(jumpTime) * JumpHeight;
        });
    */

    Entity* planeEntity = world.CreateEntity();
    planeEntity->transform.position = glm::vec3(0.0f);
    planeEntity->transform.scale = glm::vec3(8.0f, 0.1f, 8.0f);
    Material planeMaterial = assetRegistry.GetSolidMaterialInstance();
    planeMaterial.color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    planeEntity->AddComponent<MeshRenderer>(&assetRegistry.GetCubeMesh(), planeMaterial);

    // Directional light
    Entity* lightEntity = world.CreateEntity();
    DirectionalLight& light = lightEntity->AddComponent<DirectionalLight>();
    light.direction = glm::vec3(0.35f, -1.0f, 0.45f);
    light.intensity = 1.3f;

    // CPU skinning test
    const std::int16_t parents[] = {-1, 0};

    dvl::Transform bindPose[BoneCount];
    bindPose[0].translation = dvl::Vec4(0.5f, 0.0f, 0.0f, 0.0f);
    bindPose[1].translation = dvl::Vec4(0.0f, 1.0f, 0.0f, 0.0f);

    dvl::Mat4 inverseBindMatrices[BoneCount];
    inverseBindMatrices[0] = dvl::Mat4::Translation(dvl::Vec3(-0.5f, 0.0f, 0.0f));
    inverseBindMatrices[1] = dvl::Mat4::Translation(dvl::Vec3(-0.5f, -1.0f, 0.0f));

    const dvl::Skeleton skeleton
    {
        BoneCount,
        parents,
        inverseBindMatrices
    };

    const CpuSkinVertex bindVertices[AnimatedVertexCount] =
    {
        {{0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}},

        {{1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}},

        {{0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.5f}, {0, 1, 0, 0}, {0.5f, 0.5f, 0.0f, 0.0f}},

        {{1.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.5f}, {0, 1, 0, 0}, {0.5f, 0.5f, 0.0f, 0.0f}},

        {{0.0f, 2.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f}, {1, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}},

        {{1.0f, 2.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f}, {1, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}}
    };

    VertexPositionNormalUV animatedVertices[AnimatedVertexCount];
    for (std::size_t vertexIndex = 0; vertexIndex < AnimatedVertexCount; vertexIndex++)
    {
        animatedVertices[vertexIndex] = bindVertices[vertexIndex].bindVertex;
    }

    const std::uint16_t animatedIndices[] =
    {
        0, 2, 3,
        0, 3, 1,
        2, 4, 5,
        2, 5, 3
    };

    MeshDesc animatedMeshDesc = {};
    animatedMeshDesc.vertexData = animatedVertices;
    animatedMeshDesc.vertexDataSize = sizeof(animatedVertices);
    animatedMeshDesc.indices = animatedIndices;
    animatedMeshDesc.indexCount = static_cast<unsigned int>(sizeof(animatedIndices) / sizeof(animatedIndices[0]));
    animatedMeshDesc.vertexBufferUsage = dvl::BufferUsage::Dynamic;

    Mesh cpuSkinMesh = {};
    if (!renderer.CreateMesh(animatedMeshDesc, cpuSkinMesh))
    {
        dvl::Log(dvl::LogLevel::Error, "Failed to create CPU-skinned mesh");
        assetRegistry.Shutdown(renderer);
        return 1;
    }

    Entity* wireframeAnimatedEntity = world.CreateEntity();
    Material wireframeAnimatedMaterial = assetRegistry.GetWireframeMaterialInstance();
    wireframeAnimatedMaterial.color = glm::vec4(0.243f, 0.624f, 0.631f, 1.0f);
    MeshRenderer& wireframeAnimatedMeshRenderer = wireframeAnimatedEntity->AddComponent<MeshRenderer>(&cpuSkinMesh, wireframeAnimatedMaterial);
    wireframeAnimatedMeshRenderer.localTransform.position.x = -0.5f;

    Entity* solidAnimatedEntity = world.CreateEntity();
    Material solidAnimatedMaterial = assetRegistry.GetSolidMaterialInstance();
    solidAnimatedMaterial.color = glm::vec4(0.243f, 0.624f, 0.631f, 1.0f);
    MeshRenderer& solidAnimatedMeshRenderer = solidAnimatedEntity->AddComponent<MeshRenderer>(&cpuSkinMesh, solidAnimatedMaterial);
    solidAnimatedMeshRenderer.localTransform.position.x = -0.5f;

    const glm::vec3 planeTopCenter = planeEntity->transform.position + glm::vec3(0.0f, planeEntity->transform.scale.y * 0.5f, 0.0f);
    glm::vec3 cameraFacingDirection = cameraEntity->transform.position - planeTopCenter;
    cameraFacingDirection.y = 0.0f;
    const glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFacingDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    constexpr float AnimatedEntitySpacing = 1.5f;

    wireframeAnimatedEntity->transform.position = planeTopCenter - cameraRight * (AnimatedEntitySpacing * 0.5f);
    wireframeAnimatedEntity->transform.LookDirection(cameraFacingDirection);

    solidAnimatedEntity->transform.position = planeTopCenter + cameraRight * (AnimatedEntitySpacing * 0.5f);
    solidAnimatedEntity->transform.LookDirection(cameraFacingDirection);

    /*
    float rotationAngle = 0.0f;
    */
    float animationTime = 0.0f;

    while (true)
    {
        dvl::Time::Update();
        dvl::Input::Update();

        const float deltaTime = dvl::Time::GetDeltaTime();
        animationTime += deltaTime;

        dvl::Tweener::Update(deltaTime);

        // Gameplay logic
        {
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

            /*
            targetEntity->transform.rotation.y = rotationAngle;
            rotationAngle += 0.025f;
            */

            // TODO: Create a dirty flag system in getters
            mainCamera.UpdateViewMatrix();
        }

        // Animated skinmesh test
        {
            dvl::Transform animatedPose[BoneCount] = {bindPose[0], bindPose[1]};
            animatedPose[0].rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 1.0f, 0.0f), std::sin(animationTime) * 0.5f);
            animatedPose[1].rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 1.0f, 0.0f), std::sin(animationTime));

            dvl::Mat4 worldPose[BoneCount];
            dvl::Mat4 skinningMatrices[BoneCount];

            dvl::LocalToWorld(skeleton, animatedPose, dvl::Mat4::Identity(), worldPose);
            dvl::ComputeSkinningMatrices(skeleton, worldPose, skinningMatrices);

            SkinVerticesCpu(bindVertices, AnimatedVertexCount, skinningMatrices, animatedVertices);
            renderer.UpdateMeshVertices(cpuSkinMesh, animatedVertices, sizeof(animatedVertices));
        }

        renderer.BeginFrame(glm::vec4(0.32f, 0.45f, 0.65f, 1.0f));
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

    renderer.DestroyMesh(cpuSkinMesh);
    assetRegistry.Shutdown(renderer);

    return 0;
}
