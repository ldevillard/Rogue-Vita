#include "engine/core/entity.h"

#include <algorithm>
#include <dvl/log/log.h>

#include "engine/core/entity_ref.h"

Entity::Entity(World& world)
    : _world(world)
{
}

Entity::~Entity()
{
    for (std::unique_ptr<Component>& component : _components)
    {
        _world.UnRegisterComponent(component.get());
    }
}

World& Entity::GetWorld() const
{
    return _world;
}

EntityRef Entity::ToRef() const
{
    return EntityRef { id };
}

Component* Entity::AttachComponent(std::unique_ptr<Component> component)
{
    if (component == nullptr)
        return nullptr;

    if (component->GetEntity() != this)
    {
        dvl::Log(dvl::LogLevel::Error, "Cannot attach a component owned by another entity");
        return nullptr;
    }

    Component* result = component.get();

    _components.push_back(std::move(component));
    _world.RegisterComponent(result);

    return result;
}

const std::vector<std::unique_ptr<Component>>& Entity::GetComponents() const
{
    return _components;
}

void Entity::SetParent(Entity* parent)
{
    Entity* currentParent = GetParent();

    if (parent == currentParent)
        return;

    if (parent != nullptr)
    {
        for (Entity* ancestor = parent; ancestor != nullptr; ancestor = ancestor->GetParent())
        {
            if (ancestor == this)
            {
                dvl::Log(dvl::LogLevel::Error, "Cannot create a cycle in the entity hierarchy");
                return;
            }
        }
    }

    if (currentParent != nullptr)
    {
        _parent.id = 0;
        currentParent->RemoveChild(this);
    }

    if (parent == nullptr)
    {
        _parent.id = 0;
        return;
    }

    _parent = parent->ToRef();
    parent->AddChild(this);
}

Entity* Entity::GetParent() const
{
    return _parent.Get(_world);
}

void Entity::AddChild(Entity* child)
{
    if (child == nullptr)
        return;

    if (child->GetParent() != this)
    {
        child->SetParent(this);
        return;
    }

    const auto existing = std::find_if(_children.begin(), _children.end(), [child](const EntityRef& childRef)
    {
        return childRef.id == child->id;
    });

    if (existing != _children.end())
        return;

    _children.push_back(child->ToRef());
}

void Entity::RemoveChild(Entity* child)
{
    if (child == nullptr)
        return;

    if (&child->GetWorld() != &_world)
        return;

    if (child->GetParent() == this)
    {
        child->SetParent(nullptr);
        return;
    }

    _children.erase(std::remove_if(_children.begin(), _children.end(), [child](const EntityRef& childRef)
    {
        return childRef.id == child->id;
    }), _children.end());
}

const std::vector<EntityRef>& Entity::GetChildren() const
{
    return _children;
}

dvl::Mat4 Entity::GetWorldMatrix() const
{
    Entity* parent = GetParent();

    if (parent == nullptr)
    {
        return transform.GetMatrix();
    }

    return parent->GetWorldMatrix() * transform.GetMatrix();
}
