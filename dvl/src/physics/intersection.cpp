#include "dvl/physics/intersection.h"

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
}
