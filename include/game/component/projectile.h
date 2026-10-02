#pragma once

#include <dvl/math/vec.h>

#include "engine/component/behavior.h"

class Projectile : public Behavior
{
public:
    Projectile(Entity& entity);
    COMPONENT_TYPES(Projectile, Behavior, Component)
    COMPONENT_FIELDS(Behavior, speed, detectionStep, lifetime)

    // TODO: Make launch in the Start() method when prefab system will be available
    void Launch();
    void Update(float deltaTime) override;

    float speed = 10.0f;
    float detectionStep = 0.1f;
    float lifetime = 5.0f;

private:
    bool _isLaunched = false;
    float _remainingLifetime = 0.0f;
    dvl::Vec3 _direction;
};