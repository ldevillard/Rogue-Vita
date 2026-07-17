#pragma once

class Entity;

class Component
{
public:
    virtual ~Component() = default;

    virtual bool IsValid() const = 0;

    const Entity* GetEntity() const;

protected:
    Entity* entity = nullptr;

private:
    friend class Entity;

    void SetEntity(Entity* entity);
};
