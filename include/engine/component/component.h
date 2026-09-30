#pragma once

#include <cassert>
#include <memory>
#include <tuple>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <vector>

#include "engine/component/component_ref.h"
#include "engine/core/field_copy.h"

class Entity;
class Transform;
class World;

class Component
{
public:
    Component(Entity& entity);
    virtual ~Component() = default;

    virtual void Start();

    virtual std::unique_ptr<Component> CreateEmpty(Entity& owner) const;
    virtual void CopyFrom(const Component& source, const ReferenceMapping& remap);

    std::tuple<> Fields();
    std::tuple<> Fields() const;
    
    const Entity* GetEntity() const;
    const Transform& GetTransform() const;
    World& GetWorld() const;

    template<typename T>
    ComponentRef<T> ToRef() const
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        return ComponentRef<T>{ id };
    }

    virtual const std::vector<std::type_index>& GetTypes() const = 0;

    unsigned int id = 0;

protected:
    Entity& entity;
};

template<typename... T>
std::vector<std::type_index> MakeTypes()
{
    return { typeid(T)... };
}

// TODO: Think about an append method (by calling the parent method) to avoid the need of specify the types in the macro
// IMPORTANT: Include component's own type and every base type, ex: COMPONENT_TYPES(PlayerController, Behavior, Component)
#define COMPONENT_TYPES(Self, ...)                                                        \
    using RegisteredType = Self;                                                          \
    const std::vector<std::type_index>& GetTypes() const override                         \
    {                                                                                     \
        static const std::vector<std::type_index> types = MakeTypes<__VA_ARGS__, Self>(); \
        return types;                                                                     \
    }

#define COMPONENT_FIELDS(Base, ...)                                                       \
    auto Fields()                                                                         \
    {                                                                                     \
        return std::tuple_cat(Base::Fields(), std::tie(__VA_ARGS__));                     \
    }                                                                                     \
                                                                                          \
    auto Fields() const                                                                   \
    {                                                                                     \
        return std::tuple_cat(Base::Fields(), std::tie(__VA_ARGS__));                     \
    }                                                                                     \
                                                                                          \
    std::unique_ptr<Component> CreateEmpty(Entity& owner) const override                  \
    {                                                                                     \
        return std::make_unique<RegisteredType>(owner);                                   \
    }                                                                                     \
                                                                                          \
    void CopyFrom(const Component& source, const ReferenceMapping& remap) override        \
    {                                                                                     \
        assert(typeid(*this) == typeid(source));                                          \
        CopyFields(static_cast<const RegisteredType&>(source).Fields(), Fields(), remap); \
    }
