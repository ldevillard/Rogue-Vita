#pragma once

#include <dvl/math/vec.h>

class Collider;

struct RaycastHit 
{
    Collider* collider = nullptr;
    dvl::Vec3 point {};
    dvl::Vec3 normal {};
    float distance = 0.0f;
};