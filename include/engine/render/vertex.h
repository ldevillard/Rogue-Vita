#pragma once

#include <dvl/graphics/types.h>

struct VertexPositionColor
{
    float x, y, z;
    dvl::Color color;
};

struct VertexPositionNormalColor
{
    float x, y, z;
    float nx, ny, nz;
    dvl::Color color;
};
