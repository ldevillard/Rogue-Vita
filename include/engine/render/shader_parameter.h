#pragma once

#include <dvl/dvl.h>

#include <string>

struct ShaderParameterDesc
{
    const char* name = nullptr;
    dvl::ShaderParameterType type;
};

struct ShaderParameterBinding
{
    std::string name;
    dvl::ShaderParameterHandle handle;
};