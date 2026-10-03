#include "unit_test.h"

#include "engine/component/box_collider.h"
#include "engine/component/camera.h"
#include "engine/component/collider.h"
#include "engine/component/component.h"
#include "engine/component/directional_light.h"
#include "engine/component/mesh_renderer.h"
#include "engine/core/entity.h"
#include "engine/core/world.h"

namespace
{
    constexpr float Tolerance = 0.0001f;
}

DVL_TEST(WorldCreatesEntitiesWithUniqueResolvableIds)
{
    World world;
    Entity* first = world.CreateEntity();
    Entity* second = world.CreateEntity();

    DVL_EXPECT_TRUE(first->id != 0);
    DVL_EXPECT_TRUE(second->id != 0);
    DVL_EXPECT_TRUE(first->id != second->id);
    DVL_EXPECT_TRUE(world.FindEntity(first->id) == first);
    DVL_EXPECT_TRUE(world.FindEntity(second->id) == second);
    DVL_EXPECT_EQ(world.GetEntities().size(), static_cast<std::size_t>(2));

    return true;
}

DVL_TEST(WorldRegistersComponentsByConcreteAndBaseType)
{
    World world;
    Entity* entity = world.CreateEntity();
    BoxCollider& component = entity->AddComponent<BoxCollider>();

    const std::vector<BoxCollider*> concreteComponents = world.GetComponents<BoxCollider>();
    const std::vector<Collider*> colliderComponents = world.GetComponents<Collider>();
    const std::vector<Component*> baseComponents = world.GetComponents<Component>();

    DVL_EXPECT_EQ(concreteComponents.size(), static_cast<std::size_t>(1));
    DVL_EXPECT_EQ(colliderComponents.size(), static_cast<std::size_t>(1));
    DVL_EXPECT_EQ(baseComponents.size(), static_cast<std::size_t>(1));
    DVL_EXPECT_TRUE(concreteComponents[0] == &component);
    DVL_EXPECT_TRUE(colliderComponents[0] == &component);
    DVL_EXPECT_TRUE(baseComponents[0] == &component);
    DVL_EXPECT_TRUE(world.FindComponent(component.id) == &component);
    DVL_EXPECT_TRUE(entity->GetComponent<BoxCollider>() == &component);
    DVL_EXPECT_TRUE(entity->GetComponent<Collider>() == &component);

    return true;
}

DVL_TEST(WorldStartsPendingComponents)
{
    World world;
    Entity* entity = world.CreateEntity();
    Camera& camera = entity->AddComponent<Camera>(800.0f, 600.0f, Camera::Orthographic);

    DVL_EXPECT_EQ(camera.GetProjectionMatrix()[0][0], 1.0f);
    DVL_EXPECT_EQ(camera.GetProjectionMatrix()[1][1], 1.0f);

    world.StartPendingComponents();

    DVL_EXPECT_NEAR(camera.GetProjectionMatrix()[0][0], 0.375f, Tolerance);
    DVL_EXPECT_NEAR(camera.GetProjectionMatrix()[1][1], 0.5f, Tolerance);

    const dvl::Mat4 projection = camera.GetProjectionMatrix();
    world.StartPendingComponents();
    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
            DVL_EXPECT_EQ(camera.GetProjectionMatrix()[column][row], projection[column][row]);
    }

    return true;
}

DVL_TEST(WorldDestroyEntityIsDeferredUntilFlush)
{
    World world;
    Entity* entity = world.CreateEntity();
    BoxCollider& component = entity->AddComponent<BoxCollider>();
    const EntityRef entityReference = entity->ToRef();
    const ComponentRef<BoxCollider> componentReference = component.ToRef<BoxCollider>();

    world.DestroyEntity(entityReference);

    DVL_EXPECT_TRUE(entityReference.Get(world) == entity);
    DVL_EXPECT_TRUE(componentReference.Get(world) == &component);

    world.FlushDestroyedEntities();

    DVL_EXPECT_TRUE(entityReference.Get(world) == nullptr);
    DVL_EXPECT_TRUE(componentReference.Get(world) == nullptr);
    DVL_EXPECT_TRUE(world.GetComponents<BoxCollider>().empty());

    return true;
}

DVL_TEST(WorldDestroyingAParentDestroysItsWholeSubtree)
{
    World world;
    Entity* root = world.CreateEntity();
    Entity* child = world.CreateEntity();
    Entity* grandchild = world.CreateEntity();
    Entity* unrelated = world.CreateEntity();
    child->SetParent(root);
    grandchild->SetParent(child);

    const EntityRef rootReference = root->ToRef();
    const EntityRef childReference = child->ToRef();
    const EntityRef grandchildReference = grandchild->ToRef();
    const EntityRef unrelatedReference = unrelated->ToRef();

    world.DestroyEntity(rootReference);
    world.FlushDestroyedEntities();

    DVL_EXPECT_TRUE(rootReference.Get(world) == nullptr);
    DVL_EXPECT_TRUE(childReference.Get(world) == nullptr);
    DVL_EXPECT_TRUE(grandchildReference.Get(world) == nullptr);
    DVL_EXPECT_TRUE(unrelatedReference.Get(world) == unrelated);
    DVL_EXPECT_EQ(world.GetEntities().size(), static_cast<std::size_t>(1));

    return true;
}

DVL_TEST(WorldDestroyingAChildRemovesItFromItsParent)
{
    World world;
    Entity* parent = world.CreateEntity();
    Entity* child = world.CreateEntity();
    child->SetParent(parent);

    world.DestroyEntity(child->ToRef());
    world.FlushDestroyedEntities();

    DVL_EXPECT_TRUE(parent->GetChildren().empty());
    DVL_EXPECT_EQ(world.GetEntities().size(), static_cast<std::size_t>(1));

    return true;
}

DVL_TEST(WorldIgnoresDuplicateDestroyRequests)
{
    World world;
    Entity* entity = world.CreateEntity();
    const EntityRef reference = entity->ToRef();

    world.DestroyEntity(reference);
    world.DestroyEntity(reference);
    world.FlushDestroyedEntities();
    world.FlushDestroyedEntities();

    DVL_EXPECT_TRUE(reference.Get(world) == nullptr);
    DVL_EXPECT_TRUE(world.GetEntities().empty());

    return true;
}

DVL_TEST(WorldInstantiateClonesHierarchyTransformsAndComponentFields)
{
    World world;
    Entity* externalParent = world.CreateEntity();
    Entity* sourceRoot = world.CreateEntity();
    Entity* sourceChild = world.CreateEntity();
    Entity* sourceSibling = world.CreateEntity();
    Entity* sourceGrandchild = world.CreateEntity();

    sourceRoot->SetParent(externalParent);
    sourceChild->SetParent(sourceRoot);
    sourceSibling->SetParent(sourceRoot);
    sourceGrandchild->SetParent(sourceChild);

    sourceRoot->transform.position = dvl::Vec3(1.0f, 0.0f, 0.0f);
    sourceChild->transform.position = dvl::Vec3(0.0f, 2.0f, 0.0f);
    sourceSibling->transform.position = dvl::Vec3(0.0f, 0.0f, 3.0f);
    sourceGrandchild->transform.scale = dvl::Vec3(2.0f, 3.0f, 4.0f);

    DirectionalLight& rootLight = sourceRoot->AddComponent<DirectionalLight>();
    rootLight.direction = dvl::Vec3(1.0f, -2.0f, 3.0f);
    rootLight.color = dvl::Vec3(0.2f, 0.4f, 0.6f);
    rootLight.intensity = 2.5f;

    BoxCollider& childCollider = sourceChild->AddComponent<BoxCollider>(
        dvl::Vec3(1.0f, 2.0f, 3.0f), dvl::Vec3(4.0f, 5.0f, 6.0f));

    MeshRenderer& siblingRenderer = sourceSibling->AddComponent<MeshRenderer>();
    siblingRenderer.meshHandle.id = 17;
    siblingRenderer.localTransform.position = dvl::Vec3(7.0f, 8.0f, 9.0f);
    siblingRenderer.material.color = dvl::Vec4(0.1f, 0.2f, 0.3f, 0.4f);
    siblingRenderer.material.unlit = true;

    DirectionalLight& grandchildLight = sourceGrandchild->AddComponent<DirectionalLight>();
    grandchildLight.intensity = 4.0f;

    Entity* clonedRoot = world.Instantiate(*sourceRoot);
    DVL_EXPECT_TRUE(clonedRoot != sourceRoot);
    DVL_EXPECT_TRUE(clonedRoot->GetParent() == nullptr);
    DVL_EXPECT_EQ(clonedRoot->GetChildren().size(), static_cast<std::size_t>(2));

    Entity* clonedChild = clonedRoot->GetChildren()[0].Get(world);
    Entity* clonedSibling = clonedRoot->GetChildren()[1].Get(world);
    Entity* clonedGrandchild = clonedChild->GetChildren()[0].Get(world);
    DVL_EXPECT_TRUE(clonedChild->GetParent() == clonedRoot);
    DVL_EXPECT_TRUE(clonedSibling->GetParent() == clonedRoot);
    DVL_EXPECT_TRUE(clonedGrandchild->GetParent() == clonedChild);
    DVL_EXPECT_EQ(clonedRoot->transform.position.x, 1.0f);
    DVL_EXPECT_EQ(clonedChild->transform.position.y, 2.0f);
    DVL_EXPECT_EQ(clonedSibling->transform.position.z, 3.0f);
    DVL_EXPECT_EQ(clonedGrandchild->transform.scale.x, 2.0f);
    DVL_EXPECT_EQ(clonedGrandchild->transform.scale.y, 3.0f);
    DVL_EXPECT_EQ(clonedGrandchild->transform.scale.z, 4.0f);

    DirectionalLight* clonedRootLight = clonedRoot->GetComponent<DirectionalLight>();
    BoxCollider* clonedChildCollider = clonedChild->GetComponent<BoxCollider>();
    MeshRenderer* clonedSiblingRenderer = clonedSibling->GetComponent<MeshRenderer>();
    DirectionalLight* clonedGrandchildLight = clonedGrandchild->GetComponent<DirectionalLight>();
    DVL_EXPECT_TRUE(clonedRootLight != nullptr);
    DVL_EXPECT_TRUE(clonedChildCollider != nullptr);
    DVL_EXPECT_TRUE(clonedSiblingRenderer != nullptr);
    DVL_EXPECT_TRUE(clonedGrandchildLight != nullptr);
    DVL_EXPECT_TRUE(clonedRootLight != &rootLight);
    DVL_EXPECT_TRUE(clonedChildCollider != &childCollider);
    DVL_EXPECT_TRUE(clonedSiblingRenderer != &siblingRenderer);
    DVL_EXPECT_TRUE(clonedGrandchildLight != &grandchildLight);

    DVL_EXPECT_EQ(clonedRootLight->direction.x, 1.0f);
    DVL_EXPECT_EQ(clonedRootLight->direction.y, -2.0f);
    DVL_EXPECT_EQ(clonedRootLight->direction.z, 3.0f);
    DVL_EXPECT_EQ(clonedRootLight->color.x, 0.2f);
    DVL_EXPECT_EQ(clonedRootLight->color.y, 0.4f);
    DVL_EXPECT_EQ(clonedRootLight->color.z, 0.6f);
    DVL_EXPECT_EQ(clonedRootLight->intensity, 2.5f);
    DVL_EXPECT_EQ(clonedChildCollider->box.center.x, 1.0f);
    DVL_EXPECT_EQ(clonedChildCollider->box.center.y, 2.0f);
    DVL_EXPECT_EQ(clonedChildCollider->box.center.z, 3.0f);
    DVL_EXPECT_EQ(clonedChildCollider->box.size.x, 4.0f);
    DVL_EXPECT_EQ(clonedChildCollider->box.size.y, 5.0f);
    DVL_EXPECT_EQ(clonedChildCollider->box.size.z, 6.0f);
    DVL_EXPECT_EQ(clonedSiblingRenderer->meshHandle.id, 17u);
    DVL_EXPECT_EQ(clonedSiblingRenderer->localTransform.position.x, 7.0f);
    DVL_EXPECT_EQ(clonedSiblingRenderer->localTransform.position.y, 8.0f);
    DVL_EXPECT_EQ(clonedSiblingRenderer->localTransform.position.z, 9.0f);
    DVL_EXPECT_EQ(clonedSiblingRenderer->material.color.x, 0.1f);
    DVL_EXPECT_EQ(clonedSiblingRenderer->material.color.y, 0.2f);
    DVL_EXPECT_EQ(clonedSiblingRenderer->material.color.z, 0.3f);
    DVL_EXPECT_EQ(clonedSiblingRenderer->material.color.w, 0.4f);
    DVL_EXPECT_TRUE(clonedSiblingRenderer->material.unlit);
    DVL_EXPECT_EQ(clonedGrandchildLight->intensity, 4.0f);
    DVL_EXPECT_EQ(world.GetEntities().size(), static_cast<std::size_t>(9));

    return true;
}

DVL_TEST(WorldInstantiateLeavesTheOriginalHierarchyUnchanged)
{
    World world;
    Entity* parent = world.CreateEntity();
    Entity* source = world.CreateEntity();
    Entity* child = world.CreateEntity();
    source->SetParent(parent);
    child->SetParent(source);

    Entity* clone = world.Instantiate(*source);

    DVL_EXPECT_TRUE(source->GetParent() == parent);
    DVL_EXPECT_EQ(source->GetChildren().size(), static_cast<std::size_t>(1));
    DVL_EXPECT_TRUE(source->GetChildren()[0].Get(world) == child);
    DVL_EXPECT_TRUE(clone->GetParent() == nullptr);
    DVL_EXPECT_EQ(clone->GetChildren().size(), static_cast<std::size_t>(1));
    DVL_EXPECT_TRUE(clone->GetChildren()[0].Get(world) != child);

    return true;
}

DVL_TEST(ComponentReferencesResolveOnlyMatchingTypes)
{
    World world;
    Entity* entity = world.CreateEntity();
    DirectionalLight& light = entity->AddComponent<DirectionalLight>();
    BoxCollider& collider = entity->AddComponent<BoxCollider>();
    const ComponentRef<DirectionalLight> correctReference = light.ToRef<DirectionalLight>();
    const ComponentRef<BoxCollider> wrongReference{light.id};

    DVL_EXPECT_TRUE(correctReference.Get(world) == &light);
    DVL_EXPECT_TRUE(wrongReference.Get(world) == nullptr);
    DVL_EXPECT_TRUE(collider.ToRef<BoxCollider>().Get(world) == &collider);
    DVL_EXPECT_TRUE(light.GetEntity() == entity);
    DVL_EXPECT_TRUE(&light.GetWorld() == &world);
    DVL_EXPECT_TRUE(&light.GetTransform() == &entity->transform);

    return true;
}
