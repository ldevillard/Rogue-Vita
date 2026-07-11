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
    
        for (const std::pair<const unsigned int, NativeShader>& shader : _shaders)
        {
            glDeleteProgram(shader.second.program);
        }

        _shaders.clear();
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

        const std::string message = "Created buffer handle: " + std::to_string(handle.id) + ", native ID: " 
                                        + std::to_string(buffer.id) + ", size: " + std::to_string(desc.size);
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

    static GLuint CompileShader(GLenum shaderType, const ShaderCode& code)
    {
        GLuint shader = glCreateShader(shaderType);
        
        
        if (shader == 0)
        {
            Log(LogLevel::Error, "Failed to create shader");
            return 0;
        }
        
        const GLint length = static_cast<GLint>(code.size);
        glShaderSource(shader, 1, &code.data, &length);
        glCompileShader(shader);
        
        GLint compileStatus = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
        
        if (compileStatus != GL_TRUE)
        {
            GLint infoLogLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
            
            std::string infoLog(infoLogLength, '\0');
            glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.data());
            
            Log(LogLevel::Error, ("Shader compilation failed: " + infoLog).c_str());
            
            glDeleteShader(shader);
            return 0;
        }
        
        return shader;
    }
    
    ShaderHandle VitaGLBackend::CreateShader(const ShaderDesc& desc)
    {
        if (_nextShaderHandle == ShaderHandle::Invalid)
        {
            Log(LogLevel::Error, "Shader handle limit reached");
            return {};
        }

        const GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, desc.vertex);
        if (vertexShader == 0)
        {
            return {};
        }
        
        const GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, desc.fragment);
        if (fragmentShader == 0)
        {
            glDeleteShader(vertexShader);
            return {};
        }
        
        const GLuint program = glCreateProgram();
        
        if (program == 0)
        {
            Log(LogLevel::Error, "Failed to create shader program");
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return {};
        }
        
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        GLint success = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (success != GL_TRUE)
        {
            GLint infoLogLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::string infoLog(infoLogLength, '\0');
            glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());

            Log(LogLevel::Error, ("Shader program linking failed: " + infoLog).c_str());

            glDeleteProgram(program);
            return {};
        }

        ShaderHandle handle;
        handle.id = _nextShaderHandle++;
        
        NativeShader shader;
        shader.program = program;

        _shaders.emplace(handle.id, shader);

        const std::string message = "Created shader handle: " + std::to_string(handle.id) + ", program ID: " 
                                        + std::to_string(shader.program);
        Log(LogLevel::Info, message.c_str());
        return handle;
    }

    void VitaGLBackend::DestroyShader(ShaderHandle handle)
    {
        const auto it = _shaders.find(handle.id);

        if (it == _shaders.end())
        {
            Log(LogLevel::Error, "Invalid shader handle");
            return;
        }

        NativeShader& shader = it->second;

        glDeleteProgram(shader.program);

        const std::string message = "Destroyed shader with program ID: " + std::to_string(shader.program);
        Log(LogLevel::Info, message.c_str());

        _shaders.erase(it);
    }
}
