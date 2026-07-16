#pragma once

#include <dvl/dvl.h>

#include "render_pipeline.h"

struct MaterialHandle
{
    static constexpr unsigned int Invalid = std::numeric_limits<unsigned int>::max();
    
    unsigned int id = Invalid;
    
    bool IsValid() const
    {
        return id != Invalid;
    }

    bool operator==(const MaterialHandle& other) const
    {
        return id == other.id;
    }

    struct Hasher
    {
        std::size_t operator()(const MaterialHandle& handle) const
        {
            return std::hash<unsigned int>{}(handle.id);
        }
    };
};

struct Material
{
    RenderPipelineHandle renderPipelineHandle;

    dvl::Color color {1.0f, 1.0f, 1.0f, 1.0f};
};