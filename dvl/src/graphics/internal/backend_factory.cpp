#include "backend_factory.h"

#include "../vitagl/vitagl_backend.h"

#include "dvl/log/log.h"

namespace dvl::internal
{
    std::unique_ptr<Backend> CreateBackend(GraphicsAPI api)
    {
        switch (api)
        {
            case GraphicsAPI::VitaGL:
                return std::make_unique<VitaGLBackend>();
        }

        Log(LogLevel::Error, "Unsupported graphics API");
        return nullptr;
    }
}
