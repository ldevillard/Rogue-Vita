#include "engine/system/animation_system.h"

#include <dvl/log/log.h>

#include "engine/component/animator.h"
#include "engine/core/asset_registry.h"
#include "engine/core/entity.h"
#include "engine/core/world.h"

void AnimationSystem::Update(World& world, const AssetRegistry& assetRegistry, float deltaTime) const
{
    for (const std::unique_ptr<Entity>& entity : world.GetEntities())
    {
        Animator* animator = entity->GetComponent<Animator>();
        if (animator == nullptr)
            continue;

        const Skeleton* skeleton = assetRegistry.GetSkeleton(animator->GetSkeletonHandle());
        const Animation* animation = assetRegistry.GetAnimation(animator->GetAnimationHandle());

        if (skeleton == nullptr || animation == nullptr)
        {
            dvl::Log(dvl::LogLevel::Error, "AnimationSystem failed to resolve Animator assets");
            animator->Clear();
            continue;
        }

        animator->Update(deltaTime, *skeleton, *animation);
    }
}
