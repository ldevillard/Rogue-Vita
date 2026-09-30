#pragma once

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "engine/component/component.h"
#include "engine/core/transform.h"
#include "engine/core/world.h"

class EntityRef;

class Entity
{
public:
    Entity(World& world);
    ~Entity();

    World& GetWorld() const;
    EntityRef ToRef() const;

    Component* AttachComponent(std::unique_ptr<Component> component);

    template <typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        static_assert(std::is_same_v<typename T::RegisteredType, T>, "T must declare COMPONENT_TYPES for its own type");

        std::unique_ptr<T> component = std::make_unique<T>(*this, std::forward<Args>(args)...);

        Component* result = AttachComponent(std::move(component));

        assert(result != nullptr);

        return *static_cast<T*>(result);
    }

    template <typename T>
    T* GetComponent()
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        for (const std::unique_ptr<Component>& component : _components)
        {
            if (T* matchingComponent = dynamic_cast<T*>(component.get()))
                return matchingComponent;
        }

        return nullptr;
    }

    template <typename T>
    const T* GetComponent() const
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        for (const std::unique_ptr<Component>& component : _components)
        {
            if (const T* matchingComponent = dynamic_cast<const T*>(component.get()))
                return matchingComponent;
        }

        return nullptr;
    }

    const std::vector<std::unique_ptr<Component>>& GetComponents() const;

    Transform transform;
    unsigned int id = 0;

private:
    World& _world;

    std::vector<std::unique_ptr<Component>> _components;
};
