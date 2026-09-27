#pragma once

#include <vector>

#include <dvl/math/math.h>

class Collider;
class World;

class Physics
{
public:
    static void Initialize(World* world);

    static std::vector<Collider*> OverlapSphere(const dvl::Vec3& center, float radius);

private:
    static World* _world;
};