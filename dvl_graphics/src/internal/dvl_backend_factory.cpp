#include "dvl_backend_factory.h"

#include "../backend/vitagl/vitagl_backend.h"

namespace dvl::internal
{
    std::unique_ptr<Backend> CreateBackend(GraphicsAPI api)
    {
        switch (api)
        {
            case GraphicsAPI::VitaGL:
                return std::make_unique<VitaGLBackend>();
        }

        return nullptr;
    }
}
