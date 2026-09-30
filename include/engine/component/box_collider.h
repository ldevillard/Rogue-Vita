#pragma once

#include <dvl/geometry/geometry.h>

#include "engine/component/collider.h"

class BoxCollider : public Collider
{
public:
    BoxCollider(Entity& entity, const dvl::Vec3& center = dvl::Vec3::Zero(), const dvl::Vec3& size = dvl::Vec3::One());
    
    COMPONENT_TYPES(BoxCollider, Collider, Component)
    COMPONENT_FIELDS(Collider, box)

    dvl::Box box = 
    {
        dvl::Vec3::Zero(), 
        dvl::Vec3::One()
    };
};