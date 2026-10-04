#pragma once

#include "engine/component/behavior.h"
#include "engine/component/component_ref.h"

class PointLight;

class LightFlicker : public Behavior
{
public:
    LightFlicker(Entity& entity);

    COMPONENT_TYPES(LightFlicker, Behavior, Component)
    COMPONENT_FIELDS(Behavior, factor, duration)

    void Start() override;
    void Update(float deltaTime) override;

    float factor = 1.05f;
    float duration = 0.4f;

private:
    ComponentRef<PointLight> _light;

    float _baseIntensity = 0.0f;
    float _baseRadius = 0.0f;
    
    float _t = 0.0f;
    float _direction = 1.0f;
};
