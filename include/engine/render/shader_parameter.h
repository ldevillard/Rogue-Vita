#pragma once

#include <dvl/dvl.h>

enum class ShaderParameterSemantic
{
    ViewProjection,
    CameraPosition,

    ModelMatrix,

    MaterialColor,
    AlbedoTexture,

    LightCount,
    LightDirections,
    LightColors,

    SkinningMatrices
};

struct ShaderParameterDesc
{
    const char* name = nullptr;
    dvl::ShaderParameterType type;
    ShaderParameterSemantic semantic;
};

struct ShaderParameterBinding
{
    dvl::ShaderParameterType type;
    ShaderParameterSemantic semantic;
    dvl::ShaderParameterHandle handle;
};