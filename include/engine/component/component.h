#pragma once

class Entity;

class Component
{
public:
    Component(Entity& entity);
    virtual ~Component() = default;

    const Entity* GetEntity() const;

protected:
    Entity& entity;
};
