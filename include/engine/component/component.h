#pragma once

#include <typeindex>
#include <vector>

class Entity;

class Component
{
public:
    Component(Entity& entity);
    virtual ~Component() = default;

    const Entity* GetEntity() const;

    virtual const std::vector<std::type_index>& GetTypes() const = 0;

protected:
    Entity& entity;
};

template<typename... T>
std::vector<std::type_index> MakeTypes()
{
    return { typeid(T)... };
}

// IMPORTANT: Include component's own type and every base type, ex: COMPONENT_TYPES(PlayerController, Behavior, Component)
#define COMPONENT_TYPES(Self, ...)                                                       \
    using RegisteredType = Self;                                                         \
    const std::vector<std::type_index>& GetTypes() const override                        \
    {                                                                                    \
        static const std::vector<std::type_index> types = MakeTypes<__VA_ARGS__, Self>();\
        return types;                                                                    \
    }
