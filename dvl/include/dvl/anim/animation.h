#pragma once

#include "dvl/math/transform.h"

namespace dvl
{
    struct Animation
    {
        float duration = 0.0f;
        float fps = 0.0f;
        
        int boneCount = 0;
        int frameCount = 0;
        
        const Transform* keyframes = nullptr;
    };
}