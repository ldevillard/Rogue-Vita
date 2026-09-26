#include "engine/render/primitive/line_mesh_data.h"

const VertexPosition LineMeshData::vertices[2] =
{
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, -1.0f}
};

const std::uint16_t LineMeshData::indices[2] =
{
    0, 1
};