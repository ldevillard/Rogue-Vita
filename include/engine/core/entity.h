#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "engine/component/component.h"
#include "engine/core/transform.h"

class Entity
{
public:
    template <typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        std::unique_ptr<T> component = std::make_unique<T>(std::forward<Args>(args)...);
        T& componentReference = *component;

        component->SetEntity(this);

        _components.push_back(std::move(component));

        return componentReference;
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

    Transform transform;
    unsigned int id = 0;

private:
    std::vector<std::unique_ptr<Component>> _components;
};
