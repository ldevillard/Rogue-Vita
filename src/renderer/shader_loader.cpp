#include "renderer/shader_loader.h"

#include <fstream>

bool LoadShaderSource(const std::string& vertexPath, const std::string& fragmentPath, ShaderSource& outSource)
{
    // Load vertex shader source
    std::ifstream vertexFile(vertexPath);
    if (!vertexFile.is_open())
    {
        return false;
    }
    outSource.vertex.assign((std::istreambuf_iterator<char>(vertexFile)), std::istreambuf_iterator<char>());
    vertexFile.close();

    // Load fragment shader source
    std::ifstream fragmentFile(fragmentPath);
    if (!fragmentFile.is_open())
    {
        return false;
    }
    outSource.fragment.assign((std::istreambuf_iterator<char>(fragmentFile)), std::istreambuf_iterator<char>());
    fragmentFile.close();

    return true;
}
