#include "engine/physics/physics.h"

#include <dvl/geometry/geometry.h>
#include <dvl/log/log.h>
#include <dvl/physics/physics.h>

#include "engine/component/collider.h"
#include "engine/component/box_collider.h"

#include "engine/core/transform.h"
#include "engine/core/world.h"

World* Physics::_world = nullptr;

void Physics::Initialize(World* world)
{
    _world = world;
}

std::vector<Collider*> Physics::OverlapSphere(const dvl::Vec3& center, float radius)
{
    if (_world == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "Physics is not initialized, call canceled!");
        return {};
    }

    dvl::Sphere sphere{center, radius};
    std::vector<Collider*> result = {};

    for (Collider* collider : _world->GetComponents<Collider>())
    {
        const Transform& transform = collider->GetTransform();

        const BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);
        if (boxCollider)
        {
            dvl::Obb obb
            {
                transform.TransformPoint(boxCollider->box.center),
                boxCollider->box.size * dvl::Abs(transform.scale) * 0.5f,
                transform.rotation
            };

            if (dvl::Intersects(sphere, obb))
            {
                result.push_back(collider);
            }

            continue;
        }
    }

    return result;
}