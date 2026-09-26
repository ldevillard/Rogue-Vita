#include "engine/render/primitive/wire_cube_mesh_data.h"

const VertexPosition WireCubeMeshData::vertices[8] =
{
    // Front
    {-0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},

    // Back
    { 0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    {-0.5f,  0.5f, -0.5f},
};

const std::uint16_t WireCubeMeshData::indices[24] =
{
    // Front
    0, 1, 2, 3,
    0, 2, 1, 3,

    // Back
    5, 4, 6, 7,
    5, 7, 4, 6,

    // Edges
    1, 4, 0, 5, 
    3, 6, 2, 7
};