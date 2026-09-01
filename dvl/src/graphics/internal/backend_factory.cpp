#include "backend_factory.h"

#if defined(__vita__)
#include "../vitagl/vitagl_backend.h"
#else
#include "../opengl/opengl_backend.h"
#endif

#include "dvl/log/log.h"

namespace dvl::internal
{
    std::unique_ptr<Backend> CreateBackend(GraphicsAPI api)
    {
        switch (api)
        {
#if defined(__vita__)
            case GraphicsAPI::VitaGL:
                return std::make_unique<VitaGLBackend>();
#else
            case GraphicsAPI::OpenGL:
                return std::make_unique<OpenGLBackend>();
#endif
            default:
                Log(LogLevel::Error, "Unsupported graphics API");
                return nullptr;
        }
    }
}
