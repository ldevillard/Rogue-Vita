#include "engine/render/render_pipeline.h"

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
