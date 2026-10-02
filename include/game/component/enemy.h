#pragma once

#include "engine/component/animator.h"
#include "engine/component/behavior.h"
#include "engine/component/component_ref.h"

#include "game/animation/character_animations.h"
#include "game/interface/idamageable.h"

class Enemy : public Behavior, public IDamageable
{
public:
    Enemy(Entity& entity);
    Enemy(Entity& entity, const CharacterAnimations& animations);

    COMPONENT_TYPES(Enemy, Behavior, Component)
    COMPONENT_FIELDS(Behavior, _animator, _animations)

    void Start() override;
    void Update(float deltaTime) override;

    void TakeDamage(float amount) override;

private:
    ComponentRef<Animator> _animator;
    CharacterAnimations _animations;
};