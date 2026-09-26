#pragma once

#include <cstdint>

#include "engine/render/vertex.h"

struct LineMeshData
{
    static const VertexPosition vertices[2];
    static const std::uint16_t indices[2];
};