#include "game/component/light_flicker.h"

#include <dvl/math/math.h>

#include "engine/component/point_light.h"
#include "engine/core/entity.h"

LightFlicker::LightFlicker(Entity& entity)
    : Behavior(entity)
{
}

void LightFlicker::Start()
{
    PointLight* light = entity.GetComponent<PointLight>();

    _light = light->ToRef<PointLight>();

    _baseIntensity = light->intensity;
    _baseRadius = light->radius;
}

void LightFlicker::Update(float deltaTime)
{
    PointLight* light = _light.Get(GetWorld());

    if (duration <= 0.0f)
        return;

    _t += _direction * deltaTime / duration;

    if (_t >= 1.0f)
    {
        _t = 1.0f;
        _direction = -1.0f;
    }
    else if (_t <= 0.0f)
    {
        _t = 0.0f;
        _direction = 1.0f;
    }
    
    const float multiplier = dvl::Lerp(1.0f, factor, _t);

    light->intensity = _baseIntensity * multiplier;
    light->radius = _baseRadius * multiplier;
}
