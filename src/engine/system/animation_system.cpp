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
        if (animator == nullptr || !animator->IsPlaying())
            continue;

        const Skeleton* skeleton = assetRegistry.GetSkeleton(animator->GetSkeletonHandle());

        const Animation* currentAnimation = assetRegistry.GetAnimation(animator->GetCurrentAnimationHandle());
        const Animation* nextAnimation = animator->IsTransitioning() ? assetRegistry.GetAnimation(animator->GetNextAnimationHandle()) : nullptr;

        if (skeleton == nullptr || currentAnimation == nullptr || (animator->IsTransitioning() && nextAnimation == nullptr))
        {
            dvl::Log(dvl::LogLevel::Error, "AnimationSystem failed to resolve Animator assets");
            animator->Stop();
            continue;
        }

        animator->Update(deltaTime, *skeleton, *currentAnimation, nextAnimation);
    }
}
