#pragma once

#include <cstdint>

#include "engine/render/vertex.h"

struct SphereMeshData
{
    static const VertexPositionNormalUV vertices[43];
    static const std::uint16_t indices[144];
};
