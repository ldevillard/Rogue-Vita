#pragma once

#include <dvl/tween/tween.h>

#include "engine/component/animator.h"
#include "engine/component/behavior.h"
#include "engine/component/component_ref.h"
#include "engine/core/entity_ref.h"

#include "game/animation/character_animations.h"

class Camera;

class PlayerController : public Behavior
{
public:
    PlayerController(Entity& entity);
    PlayerController(Entity& entity, EntityRef camera, ComponentRef<Animator> animator, const CharacterAnimations& animations, EntityRef bulletPrefab);
    ~PlayerController() override;
    
    COMPONENT_TYPES(PlayerController, Behavior, Component)
    COMPONENT_FIELDS(Behavior, moveSpeed, rotationSpeed, detectionRadius, minDistance, 
                        dashDistance, dashDuration, animationTransitionDuration, _camera, _animator, _animations, _bulletPrefab)

    void Start() override;
    void Update(float deltaTime) override;

    float moveSpeed = 5.0f;
    float rotationSpeed = 20.0f;
    float detectionRadius = 2.5f;
    float minDistance = 1.0f;

    float dashDistance = 3.0f;
    float dashDuration = 0.15f;
    float animationTransitionDuration = 0.1f;

private:
    void dash();
    void attack();

    void drawDebugFov();

    EntityRef _camera;
    ComponentRef<Animator> _animator;
    EntityRef _bulletPrefab;
    
    CharacterAnimations _animations;

    bool _isAttacking = false;
    dvl::Quat _aimRotation;
    
    dvl::ITween* _dashTween = nullptr;
};
