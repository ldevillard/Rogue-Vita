#pragma once

#include <dvl/dvl.h>

#include "render_pipeline.h"

struct Material
{
    RenderPipeline pipeline;

    dvl::Color color {1.0f, 1.0f, 1.0f, 1.0f};
};