#pragma once

#include <dvl/math/math.h>

// TODO: Compute matrices and vectors once per frame, maybe a dirty flag system may work
class Transform
{
public:
    dvl::Vec3 position;
    // TODO: Support quat to avoid gimball lock
    dvl::Vec3 rotation;
    dvl::Vec3 scale = dvl::Vec3(1.0f, 1.0f, 1.0f);

    dvl::Vec3 GetForward() const;
    dvl::Vec3 GetRight() const;
    dvl::Vec3 GetUp() const;
    dvl::Mat4 GetMatrix() const;

    // TODO: Update with quat when it will be available
    void LookAt(const dvl::Vec3& target);
    void LookDirection(const dvl::Vec3& direction);
};
