#pragma once

#include "dvl/math/math.h"
#include "skeleton.h"

namespace dvl
{
    void LocalToWorld(const Skeleton& skeleton, const Transform* localPose, const Mat4& rootTransform, Mat4* outPose);
}