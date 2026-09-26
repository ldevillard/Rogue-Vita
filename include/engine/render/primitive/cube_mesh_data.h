#pragma once

#include <cstdint>

#include "engine/render/vertex.h"

struct CubeMeshData
{
    static const VertexPositionNormalUV vertices[24];
    static const std::uint16_t indices[36];
};