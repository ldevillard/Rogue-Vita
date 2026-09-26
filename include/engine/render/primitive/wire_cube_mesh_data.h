#pragma once

#include <cstdint>

#include "engine/render/vertex.h"

struct WireCubeMeshData
{
    static const VertexPosition vertices[8];
    static const std::uint16_t indices[24];
};