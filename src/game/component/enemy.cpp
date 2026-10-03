#include "game/component/enemy.h"

#include <dvl/tween/tweener.h>

#include "engine/core/entity.h"

Enemy::Enemy(Entity& entity)
    : Behavior(entity)
{
}

Enemy::Enemy(Entity& entity, const CharacterAnimations& animations)
    : Behavior(entity), _animations(animations)
{
}

void Enemy::Start()
{
    Animator* animator = entity.GetComponent<Animator>();

    animator->Play(_animations.idle);
    _animator = animator->ToRef<Animator>();
    _baseScale = entity.transform.scale;
}

void Enemy::Update(float)
{
    Animator* animator = _animator.Get(GetWorld());

    if (animator->IsFinished())
    {
        animator->Play(_animations.idle, 0.1f);
    }
}

void Enemy::TakeDamage(float)
{
    _animator.Get(GetWorld())->Restart(_animations.takeDamage, 0.1f);
    
    dvl::Tweener::Create(_baseScale * 1.25f, _baseScale, 0.2f, dvl::Easing::OutBack).OnUpdate([this](const dvl::Vec3& value)
    {
        entity.transform.scale = value;
    });
}