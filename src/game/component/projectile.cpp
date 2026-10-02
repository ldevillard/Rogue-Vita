#include "game/component/projectile.h"

#include <algorithm>

#include "engine/component/collider.h"
#include "engine/core/entity.h"
#include "engine/debug/debug_draw.h"
#include "engine/physics/physics.h"
#include "engine/physics/raycast_hit.h"

#include "game/interface/idamageable.h"

Projectile::Projectile(Entity& entity)
    : Behavior(entity)
{
}

void Projectile::Launch()
{
    _direction = entity.transform.GetForward();
    _remainingLifetime = lifetime;
    _isLaunched = true;
}

void Projectile::Update(float deltaTime)
{
    if (!_isLaunched)
        return;

    _remainingLifetime -= deltaTime;
    if (_remainingLifetime <= 0.0f)
    {
        GetWorld().DestroyEntity(entity.id);
        return;
    }

    const float distance = speed * deltaTime;
    const float detectionDistance = std::max(detectionStep, distance);

    RaycastHit hit;
    if (Physics::Raycast(entity.transform.position, _direction, hit, detectionDistance))
    {
        if (IDamageable* damageable = hit.collider->GetEntity()->GetInterface<IDamageable>())
        {
            damageable->TakeDamage(1.0f);
        }

        _direction = dvl::Reflect(_direction, hit.normal);
        return;
    }

    DebugDraw::DrawLine(entity.transform.position, entity.transform.position + _direction * detectionDistance, dvl::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

    entity.transform.position += _direction * distance;
}