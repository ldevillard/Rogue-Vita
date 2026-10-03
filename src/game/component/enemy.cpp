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

    _health = entity.GetComponent<Health>()->ToRef<Health>();
}

void Enemy::Update(float)
{
    if (_health.Get(GetWorld())->IsDead())
    {
        return;
    }

    Animator* animator = _animator.Get(GetWorld());

    if (animator->IsFinished())
    {
        animator->Play(_animations.idle, 0.1f);
    }
}

void Enemy::TakeDamage(float amount)
{
    if (_health.Get(GetWorld())->IsDead())
    {
        return;
    }

    _animator.Get(GetWorld())->Restart(_animations.takeDamage, 0.1f);
    _health.Get(GetWorld())->Damage(amount);

    dvl::Tweener::Create(_baseScale * 1.25f, _baseScale, 0.2f, dvl::Easing::OutBack).OnUpdate([this](const dvl::Vec3& value)
    {
        entity.transform.scale = value;
    });

    if (_health.Get(GetWorld())->IsDead())
    {
        _animator.Get(GetWorld())->Play(_animations.die, 0.1f);
        
        dvl::Tweener::Create(_baseScale, dvl::Vec3::Zero(), 0.5f, dvl::Easing::InBack)
        .Delay(0.5f)
        .OnUpdate([this](const dvl::Vec3& value)
        {
            entity.transform.scale = value;
        }).OnComplete([this]()
        {
            GetWorld().DestroyEntity(entity.ToRef());
        });
        
        return;
    }
}