#pragma once

#include <cstdint>

#include "engine/render/vertex.h"

struct CircleMeshData
{
    static const VertexPosition vertices[32];
    static const std::uint16_t indices[64];
};
