#pragma once

#include <glm/glm.hpp>

struct VertexPositionColor
{
    float x, y, z;
    glm::vec4 color;
};

struct VertexPositionNormalColor
{
    float x, y, z;
    float nx, ny, nz;
    glm::vec4 color;
};

struct VertexPositionNormalUV
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};
