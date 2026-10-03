#pragma once

#include "engine/component/component.h"

class Entity;

class Health : public Component
{
public:
    Health(Entity& entity);
    Health(Entity& entity, float maxHealth);

    COMPONENT_TYPES(Health, Component);
    COMPONENT_FIELDS(Component, _health, _maxHealth);

    void Damage(float amount);
    void Heal(float amount);

    float GetHealth() const;
    float GetMaxHealth() const;
    bool IsDead() const;
    
private:
    float _health = 100.0f;
    float _maxHealth = 100.0f;
};