#include "engine/component/box_collider.h"

#include "engine/component/collider.h"

BoxCollider::BoxCollider(Entity& entity, const dvl::Vec3& center, const dvl::Vec3& size)
    : Collider(entity), box{center, size}
{    
}
