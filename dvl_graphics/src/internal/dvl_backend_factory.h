#pragma once

#include <dvl_types.h>

#include <memory>

namespace dvl::internal
{
    class Backend;

    std::unique_ptr<Backend> CreateBackend(GraphicsAPI api);
}
