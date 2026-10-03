#pragma once

#include "engine/component/behavior.h"
#include "engine/component/mesh_renderer.h"

#include "game/component/health.h"

class HealthDisplayer : public Behavior
{
public:
    HealthDisplayer(Entity& entity);
    HealthDisplayer(Entity& entity, ComponentRef<Health> health);

    COMPONENT_TYPES(HealthDisplayer, Behavior, Component)
    COMPONENT_FIELDS(Behavior, _health, _meshRenderer, _baseScaleX)

    void Start() override;
    // TODO: Update health displayer using event system when it will be available
    void Update(float deltaTime) override;

private:
    ComponentRef<Health> _health;
    ComponentRef<MeshRenderer> _meshRenderer;

    float _baseScaleX;
};