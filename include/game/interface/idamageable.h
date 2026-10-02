#pragma once

class IDamageable
{
public:
    virtual ~IDamageable() = default;
    virtual void TakeDamage(float amount) = 0;
};