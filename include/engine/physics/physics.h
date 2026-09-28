#pragma once

#include <vector>

#include <dvl/math/math.h>

class Collider;
struct RaycastHit;
class World;

class Physics
{
public:
    static void Initialize(World* world);

    static std::vector<Collider*> OverlapSphere(const dvl::Vec3& center, float radius);
    static bool Raycast(dvl::Vec3 origin, dvl::Vec3 direction, RaycastHit& outHit, float maxDistance = dvl::Infinity);

private:
    static World* _world;
};