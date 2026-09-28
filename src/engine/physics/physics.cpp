#include "engine/physics/physics.h"

#include <dvl/geometry/geometry.h>
#include <dvl/log/log.h>
#include <dvl/physics/physics.h>

#include "engine/component/collider.h"
#include "engine/component/box_collider.h"

#include "engine/core/transform.h"
#include "engine/core/world.h"

#include "engine/physics/raycast_hit.h"

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

        // TODO: Add other types of collider support
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
        }
    }
    return result;
}

bool Physics::Raycast(dvl::Vec3 origin, dvl::Vec3 direction, RaycastHit& outHit, float maxDistance)
{
    outHit = {};

    if (_world == nullptr)
    {
        dvl::Log(dvl::LogLevel::Error, "Physics is not initialized, call canceled!");
        return false;
    }

    if (maxDistance < 0.0f || direction.LengthSquared() == 0.0f)
        return false;

    direction = direction.Normalized();
    const dvl::Ray ray
    {
        origin, 
        direction
    };
    
    bool hit = false;
    float closestDistance = maxDistance;

    for (Collider* collider : _world->GetComponents<Collider>())
    {
        const Transform& transform = collider->GetTransform();

        // TODO: Add other types of collider support
        const BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);

        if (boxCollider)
        {
            dvl::Obb obb
            {
                transform.TransformPoint(boxCollider->box.center),
                boxCollider->box.size * dvl::Abs(transform.scale) * 0.5f,
                transform.rotation
            };

            float distance;
            dvl::Vec3 normal;
            
            if (!dvl::Intersects(ray, obb, distance, normal) || distance > closestDistance)
                continue;

            closestDistance = distance;
            hit = true;
            
            outHit.collider = collider;
            outHit.point = origin + direction * distance;
            outHit.normal = normal;
            outHit.distance = distance;
        }
    }

    return hit;
}