#pragma once

#include "engine/component/collider.h"

class BoxCollider : public Collider
{
public:
    BoxCollider(Entity& entity, const dvl::Vec3& center = dvl::Vec3::Zero(), const dvl::Vec3& size = dvl::Vec3::One());
    COMPONENT_TYPES(BoxCollider, Collider, Component)

    dvl::Vec3 center = dvl::Vec3(0.0f, 0.0f, 0.0f);
    dvl::Vec3 size = dvl::Vec3(1.0f, 1.0f, 1.0f);
};