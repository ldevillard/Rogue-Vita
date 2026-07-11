#include "vitagl_backend.h"

#include <dvl/log/log.h>

#include <vitaGL.h>

#include <string>

namespace dvl::internal
{
    bool VitaGLBackend::Initialize(const DeviceDesc& desc)
    {
        Log(LogLevel::Info, "Initializing VitaGL backend");

        vglInit(0x800000);
        
        vglWaitVblankStart(desc.vsync ? GL_TRUE : GL_FALSE);

        Viewport viewport;
        viewport.width = desc.width;
        viewport.height = desc.height;

        SetViewport(viewport);
        
        Log(LogLevel::Info, "VitaGL backend initialized");
        return true;
    }

    void VitaGLBackend::Shutdown()
    {
        for (const std::pair<const unsigned int, NativeBuffer>& buffer : _buffers)
        {
            glDeleteBuffers(1, &buffer.second.id);
        }

        _buffers.clear();
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

    void VitaGLBackend::SetViewport(const Viewport& viewport)
    {
        glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }

    BufferHandle VitaGLBackend::CreateBuffer(const BufferDesc& desc)
    {
        if (_nextBufferHandle == BufferHandle::Invalid)
        {
            Log(LogLevel::Error, "Buffer handle limit reached");
            return {};
        }

        NativeBuffer buffer;
        buffer.size = desc.size;
        
        glGenBuffers(1, &buffer.id);

        if (buffer.id == 0)
        {
            Log(LogLevel::Error, "Failed to create VitaGL buffer");
            return {};
        }

        GLenum usage = GL_STATIC_DRAW;

        switch (desc.usage)
        {
            case BufferUsage::Static:
                usage = GL_STATIC_DRAW;
                break;

            case BufferUsage::Dynamic:
                usage = GL_DYNAMIC_DRAW;
                break;
        }

        glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
        glBufferData(GL_ARRAY_BUFFER, desc.size, desc.data, usage);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        BufferHandle handle;
        handle.id = _nextBufferHandle++;

        _buffers.emplace(handle.id, buffer);

        const std::string message = "Created buffer with ID: " + std::to_string(buffer.id) + ", size: " + std::to_string(desc.size);
        Log(LogLevel::Info, message.c_str());

        return handle;
    }

    void VitaGLBackend::DestroyBuffer(BufferHandle handle)
    {
        const auto it = _buffers.find(handle.id);

        if (it == _buffers.end())
        {
            Log(LogLevel::Error, "Invalid buffer handle");
            return;
        }

        NativeBuffer& buffer = it->second;

        glDeleteBuffers(1, &buffer.id);

        const std::string message = "Destroyed buffer with ID: " + std::to_string(buffer.id);
        Log(LogLevel::Info, message.c_str());

        _buffers.erase(it);
    }
}
