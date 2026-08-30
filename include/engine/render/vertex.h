#pragma once

#include <dvl/math/math.h>

struct VertexPositionColor
{
    float x, y, z;
    dvl::Vec4 color;
};

struct VertexPositionNormalColor
{
    float x, y, z;
    float nx, ny, nz;
    dvl::Vec4 color;
};

struct VertexPositionNormalUV
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

struct SkinnedVertexPositionNormalUV
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;

    std::uint8_t boneIndices[4];
    std::uint8_t boneWeights[4];
};