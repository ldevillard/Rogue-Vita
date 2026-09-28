#include "dvl/physics/intersection.h"

#include <algorithm>

namespace dvl
{
    bool Intersects(const Sphere& sphere, const Obb& box)
    {
        const Mat4 rotation = Mat4::Rotation(box.rotation);
        const Mat4 inverseRotation = Mat4::Inverse(rotation);

        const Vec3 offset = sphere.center - box.center;
        const Vec3 localCenter = (inverseRotation * Vec4(offset.x, offset.y, offset.z, 0.0f)).XYZ();

        const Vec3 closest(Clamp(localCenter.x, -box.halfExtents.x, box.halfExtents.x),
                           Clamp(localCenter.y, -box.halfExtents.y, box.halfExtents.y),
                           Clamp(localCenter.z, -box.halfExtents.z, box.halfExtents.z));

        const Vec3 delta = localCenter - closest;
        return delta.LengthSquared() <= sphere.radius * sphere.radius;
    }

    bool Intersects(const Ray& ray, const Obb& box, float& outDistance, Vec3& outNormal)
    {
        if (ray.direction.LengthSquared() == 0.0f)
            return false;

        // Rotate a vector directly with a unit quaternion.
        const auto rotate = [](const Quat& q, const Vec3& v)
        {
            const Vec3 axis{q.x, q.y, q.z};
            const Vec3 t = Cross(axis, v) * 2.0f;
            return v + t * q.w + Cross(axis, t);
        };

        const Quat rotation = box.rotation.Normalized();
        const Quat inverseRotation = rotation.Conjugated();
        const Vec3 localOrigin = rotate(inverseRotation, ray.origin - box.center);
        const Vec3 localDirection = rotate(inverseRotation, ray.direction);

        float tEnter = -Infinity;
        float tExit = Infinity;
        Vec3 enterNormal{};
        Vec3 exitNormal{};

        const auto testAxis = [&](float origin, float direction, float extent, const Vec3& axis)
        {
            if (direction == 0.0f)
                return origin >= -extent && origin <= extent;

            float tNear = (-extent - origin) / direction;
            float tFar = (extent - origin) / direction;
            const Vec3 nearNormal = axis * (direction > 0.0f ? -1.0f : 1.0f);

            if (tNear > tFar)
                std::swap(tNear, tFar);

            if (tNear > tEnter)
            {
                tEnter = tNear;
                enterNormal = nearNormal;
            }

            if (tFar < tExit)
            {
                tExit = tFar;
                exitNormal = nearNormal * -1.0f;
            }

            return tEnter <= tExit;
        };

        if (!testAxis(localOrigin.x, localDirection.x, box.halfExtents.x, {1.0f, 0.0f, 0.0f}) ||
            !testAxis(localOrigin.y, localDirection.y, box.halfExtents.y, {0.0f, 1.0f, 0.0f}) ||
            !testAxis(localOrigin.z, localDirection.z, box.halfExtents.z, {0.0f, 0.0f, 1.0f}) ||
            tExit < 0.0f)
        {
            return false;
        }

        // A ray starting inside the box hits its exit surface.
        const bool entering = tEnter >= 0.0f;

        const Vec3 localNormal = entering ? enterNormal : exitNormal;
        
        outDistance = entering ? tEnter : tExit;
        outNormal = rotate(rotation, localNormal).Normalized();

        return true;
    }
}
