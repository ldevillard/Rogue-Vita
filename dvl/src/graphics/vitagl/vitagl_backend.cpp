#include "vitagl_backend.h"

#include <dvl/log/log.h>

#include <vitaGL.h>

namespace dvl::internal
{
    bool VitaGLBackend::Initialize(const DeviceDesc& desc)
    {
        Log(LogLevel::Info, "Initializing VitaGL backend");

        vglInit(0x800000);
        
        vglWaitVblankStart(desc.vsync ? GL_TRUE : GL_FALSE);
        glViewport(0, 0, desc.width, desc.height);

        Log(LogLevel::Info, "VitaGL backend initialized");
        return true;
    }

    void VitaGLBackend::Shutdown()
    {
    }

    void VitaGLBackend::BeginFrame(const Color& clearColor)
    {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

        glClear(GL_COLOR_BUFFER_BIT);
    }

    void VitaGLBackend::EndFrame()
    {
        vglSwapBuffers(GL_FALSE);
    }
}
