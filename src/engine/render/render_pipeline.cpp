#include "engine/render/render_pipeline.h"

dvl::ShaderParameterHandle RenderPipeline::GetParameter(std::string_view name) const
{
    for (const ShaderParameterBinding& parameter : parameters)
    {
        if (parameter.name == name)
            return parameter.handle;
    }

    dvl::Log(dvl::LogLevel::Error, "Couldn't find shader parameter!");
    return {};
}

bool RenderPipeline::IsValid() const
{
    if (!shader.IsValid() || !pipeline.IsValid())
        return false;

    for (const ShaderParameterBinding& parameter : parameters)
    {
        if (!parameter.handle.IsValid())
            return false;
    }

    return true;
}
