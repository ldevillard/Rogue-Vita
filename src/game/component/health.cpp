#include "game/component/health.h"

Health::Health(Entity& entity)
    : Component(entity)
{
}

Health::Health(Entity& entity, float maxHealth)
    : Component(entity), _health(maxHealth), _maxHealth(maxHealth)
{
}

void Health::Damage(float amount)
{
    if (amount <= 0.0f)
    {
        return;
    }

    _health = std::max(0.0f, _health - amount);
}

void Health::Heal(float amount)
{
    if (amount <= 0.0f)
    {
        return;
    }

    _health = std::min(_maxHealth, _health + amount);
}

float Health::GetHealth() const
{
    return _health;
}

float Health::GetMaxHealth() const
{
    return _maxHealth;
}

float Health::GetHealthPercentage() const
{
    if (_maxHealth <= 0.0f)
    {
        return 0.0f;
    }

    return _health / _maxHealth;
}

bool Health::IsDead() const
{
    return _health <= 0.0f;
}