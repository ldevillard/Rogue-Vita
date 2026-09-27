#pragma once

#include <cstdint>

#include "engine/render/vertex.h"

struct WireSphereMeshData
{
    static const VertexPosition vertices[96];
    static const std::uint16_t indices[192];
};
