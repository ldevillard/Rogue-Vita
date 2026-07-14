#pragma once

class Entity;

class Component
{
public:
    virtual ~Component() = default;

    virtual bool IsValid() const = 0;

    // TODO: Protect entity
    Entity* entity = nullptr;
};
