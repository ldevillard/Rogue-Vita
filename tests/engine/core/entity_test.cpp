#include "unit_test.h"

#include "engine/core/entity.h"
#include "engine/core/world.h"

namespace
{
    constexpr float Tolerance = 0.0001f;
}

DVL_TEST(EntityStartsWithoutParentOrChildren)
{
    World world;
    Entity* entity = world.CreateEntity();

    DVL_EXPECT_TRUE(entity->GetParent() == nullptr);
    DVL_EXPECT_TRUE(entity->GetChildren().empty());

    return true;
}

DVL_TEST(EntityReferenceResolvesToItsEntity)
{
    World world;
    Entity* entity = world.CreateEntity();
    const EntityRef reference = entity->ToRef();

    DVL_EXPECT_EQ(reference.id, entity->id);
    DVL_EXPECT_TRUE(reference.Get(world) == entity);
    DVL_EXPECT_TRUE(EntityRef{}.Get(world) == nullptr);

    return true;
}

DVL_TEST(EntitySetParentUpdatesBothSidesOfTheHierarchy)
{
    World world;
    Entity* parent = world.CreateEntity();
    Entity* child = world.CreateEntity();

    child->SetParent(parent);

    DVL_EXPECT_TRUE(child->GetParent() == parent);
    DVL_EXPECT_EQ(parent->GetChildren().size(), static_cast<std::size_t>(1));
    DVL_EXPECT_TRUE(parent->GetChildren()[0].Get(world) == child);

    return true;
}

DVL_TEST(EntityAddChildDoesNotCreateDuplicates)
{
    World world;
    Entity* parent = world.CreateEntity();
    Entity* child = world.CreateEntity();

    parent->AddChild(child);
    parent->AddChild(child);
    child->SetParent(parent);

    DVL_EXPECT_TRUE(child->GetParent() == parent);
    DVL_EXPECT_EQ(parent->GetChildren().size(), static_cast<std::size_t>(1));

    return true;
}

DVL_TEST(EntityRemoveChildDetachesBothSides)
{
    World world;
    Entity* parent = world.CreateEntity();
    Entity* child = world.CreateEntity();
    child->SetParent(parent);

    parent->RemoveChild(child);

    DVL_EXPECT_TRUE(child->GetParent() == nullptr);
    DVL_EXPECT_TRUE(parent->GetChildren().empty());

    return true;
}

DVL_TEST(EntityReparentingRemovesTheChildFromItsPreviousParent)
{
    World world;
    Entity* firstParent = world.CreateEntity();
    Entity* secondParent = world.CreateEntity();
    Entity* child = world.CreateEntity();

    child->SetParent(firstParent);
    child->SetParent(secondParent);

    DVL_EXPECT_TRUE(firstParent->GetChildren().empty());
    DVL_EXPECT_EQ(secondParent->GetChildren().size(), static_cast<std::size_t>(1));
    DVL_EXPECT_TRUE(secondParent->GetChildren()[0].Get(world) == child);
    DVL_EXPECT_TRUE(child->GetParent() == secondParent);

    return true;
}

DVL_TEST(EntityRejectsSelfParenting)
{
    World world;
    Entity* entity = world.CreateEntity();

    entity->SetParent(entity);

    DVL_EXPECT_TRUE(entity->GetParent() == nullptr);
    DVL_EXPECT_TRUE(entity->GetChildren().empty());

    return true;
}

DVL_TEST(EntityRejectsCyclesWithoutChangingTheHierarchy)
{
    World world;
    Entity* root = world.CreateEntity();
    Entity* child = world.CreateEntity();
    Entity* grandchild = world.CreateEntity();
    child->SetParent(root);
    grandchild->SetParent(child);

    root->SetParent(grandchild);

    DVL_EXPECT_TRUE(root->GetParent() == nullptr);
    DVL_EXPECT_TRUE(child->GetParent() == root);
    DVL_EXPECT_TRUE(grandchild->GetParent() == child);
    DVL_EXPECT_EQ(root->GetChildren().size(), static_cast<std::size_t>(1));
    DVL_EXPECT_EQ(child->GetChildren().size(), static_cast<std::size_t>(1));

    return true;
}

DVL_TEST(EntityWorldMatrixCombinesTheCompleteParentChain)
{
    World world;
    Entity* root = world.CreateEntity();
    Entity* child = world.CreateEntity();
    Entity* grandchild = world.CreateEntity();

    root->transform.position = dvl::Vec3(10.0f, 0.0f, 0.0f);
    root->transform.rotation = dvl::Quat::FromAxisAngle(dvl::Vec3(0.0f, 0.0f, 1.0f), dvl::Pi / 2.0f);
    child->transform.position = dvl::Vec3(2.0f, 0.0f, 0.0f);
    grandchild->transform.position = dvl::Vec3(0.0f, 3.0f, 0.0f);
    child->SetParent(root);
    grandchild->SetParent(child);

    const dvl::Vec4 worldOrigin = grandchild->GetWorldMatrix() * dvl::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
    DVL_EXPECT_NEAR(worldOrigin.x, 7.0f, Tolerance);
    DVL_EXPECT_NEAR(worldOrigin.y, 2.0f, Tolerance);
    DVL_EXPECT_NEAR(worldOrigin.z, 0.0f, Tolerance);
    DVL_EXPECT_NEAR(worldOrigin.w, 1.0f, Tolerance);

    return true;
}

DVL_TEST(EntityParentingKeepsTheLocalTransformUnchanged)
{
    World world;
    Entity* parent = world.CreateEntity();
    Entity* child = world.CreateEntity();
    child->transform.position = dvl::Vec3(1.0f, 2.0f, 3.0f);

    child->SetParent(parent);
    child->SetParent(nullptr);

    DVL_EXPECT_EQ(child->transform.position.x, 1.0f);
    DVL_EXPECT_EQ(child->transform.position.y, 2.0f);
    DVL_EXPECT_EQ(child->transform.position.z, 3.0f);

    return true;
}
