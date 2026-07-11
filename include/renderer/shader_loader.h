#pragma once

#include <string>

struct ShaderSource
{
    std::string vertex;
    std::string fragment;
}; 

bool LoadShaderSource(const std::string& vertexPath, const std::string& fragmentPath, ShaderSource& outSource);
