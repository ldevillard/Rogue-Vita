#include "game/component/health_displayer.h"

#include "engine/core/entity.h"

#include <dvl/math/math.h>

HealthDisplayer::HealthDisplayer(Entity& entity)
    : Behavior(entity)
{
}

HealthDisplayer::HealthDisplayer(Entity& entity, ComponentRef<Health> health)
    : Behavior(entity), _health(health)
{
}

void HealthDisplayer::Start()
{
    _meshRenderer = entity.GetComponent<MeshRenderer>()->ToRef<MeshRenderer>();
    _baseScaleX = entity.transform.scale.x;

    Update(0.0f);
}

void HealthDisplayer::Update(float)
{
    Health* health = _health.Get(GetWorld());
    if (health == nullptr)
    {
        return;
    }

    const float healthPercentage = dvl::Clamp(health->GetHealthPercentage(), 0.0f, 1.0f);

    entity.transform.scale.x = _baseScaleX * healthPercentage;

    const float red = dvl::Clamp((1.0f - healthPercentage) * 2.0f, 0.0f, 1.0f);
    const float green = dvl::Clamp(healthPercentage * 2.0f, 0.0f, 1.0f);

    _meshRenderer.Get(GetWorld())->material.color = dvl::Vec4(red, green, 0.0f, 1.0f);
}
