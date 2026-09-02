#include "opengl_backend.h"

#include "dvl/log/log.h"

#include <string>

namespace dvl::internal
{
    bool OpenGLBackend::Initialize(const DeviceDesc& desc)
    {
        Log(LogLevel::Info, "Initializing OpenGL backend");

        if (!glfwInit())
        {
            Log(LogLevel::Error, "Failed to initialize GLFW");
            return false;
        }

        _window = glfwCreateWindow(desc.width, desc.height, "OpenGL Window", nullptr, nullptr);
        if (!_window)
        {
            Log(LogLevel::Error, "Failed to create GLFW window");
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(_window);

        GLenum err = glewInit();
        if (err != GLEW_OK)
        {
            Log(LogLevel::Error, "Failed to initialize GLEW");
            return false;
        }

        glfwSwapInterval(desc.vsync ? 1 : 0);

        Viewport viewport;
        viewport.width = desc.width;
        viewport.height = desc.height;

        SetViewport(viewport);
        
        Log(LogLevel::Info, "OpenGL backend initialized");
        return true;
    }

    void OpenGLBackend::Shutdown()
    {
        _shaderParameters.clear();

        _pipelines.clear();

        for (const std::pair<const unsigned int, NativeTexture>& texture : _textures)
        {
            glDeleteTextures(1, &texture.second.id);
        }

        _textures.clear();

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

        glfwDestroyWindow(_window);
        _window = nullptr;

        glfwTerminate();
    }

    bool OpenGLBackend::ShouldClose() const
    {
        return glfwWindowShouldClose(_window) != 0;
    }

    void OpenGLBackend::BeginFrame(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);

        glClearDepthf(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLBackend::EndFrame()
    {
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    void OpenGLBackend::SetViewport(const Viewport& viewport)
    {
        glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }

    BufferHandle OpenGLBackend::CreateBuffer(const BufferDesc& desc)
    {
        NativeBuffer buffer;
        buffer.size = desc.size;
        buffer.type = desc.type;
        
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

        const GLenum target = desc.type == BufferType::Index ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;

        glBindBuffer(target, buffer.id);
        glBufferData(target, desc.size, desc.data, usage);
        glBindBuffer(target, 0);

        BufferHandle handle;
        handle.id = _nextBufferHandle++;

        _buffers.emplace(handle.id, buffer);

        const std::string message = "Created buffer handle: " + std::to_string(handle.id) + ", native ID: " 
                                        + std::to_string(buffer.id) + ", size: " + std::to_string(desc.size);
        Log(LogLevel::Info, message.c_str());

        return handle;
    }

    bool OpenGLBackend::UpdateBuffer(BufferHandle handle, const void* data, std::size_t size)
    {
        const auto it = _buffers.find(handle.id);

        if (it == _buffers.end())
        {
            Log(LogLevel::Error, "Invalid buffer handle");
            return false;
        }

        NativeBuffer& buffer = it->second;

        if (size > buffer.size)
        {
            Log(LogLevel::Error, "Data size exceeds buffer size");
            return false;
        }

        const GLenum target = buffer.type == BufferType::Index ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;

        glBindBuffer(target, buffer.id);
        glBufferSubData(target, 0, size, data);
        glBindBuffer(target, 0);

        return true;
    }

    void OpenGLBackend::DestroyBuffer(BufferHandle handle)
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
    
    ShaderHandle OpenGLBackend::CreateShader(const ShaderDesc& desc)
    {
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

    void OpenGLBackend::DestroyShader(ShaderHandle handle)
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

    PipelineHandle OpenGLBackend::CreatePipeline(const PipelineDesc& desc)
    {
        const auto shaderIt = _shaders.find(desc.shader.id);
        if (shaderIt == _shaders.end())
        {
            Log(LogLevel::Error, "Invalid shader handle");
            return {};
        }

        NativePipeline pipeline;
        pipeline.program = shaderIt->second.program;
        pipeline.vertexStride = desc.vertexStride;
        pipeline.topology = desc.topology;
        pipeline.depthStencilState = desc.depthStencilState;
        pipeline.rasterizerState = desc.rasterizerState;

        for (std::size_t index = 0; index < desc.attributeCount; ++index)
        {
            const VertexAttribute& attribute = desc.attributes[index];
            const GLint location = glGetAttribLocation(pipeline.program, attribute.name);

            if (location == -1)
            {
                const std::string message = "Attribute not found in shader: " + std::string(attribute.name);
                Log(LogLevel::Error, message.c_str());
                return {};
            }

            NativePipeline::NativeVertexAttribute nativeAttribute;
            nativeAttribute.location = location;
            nativeAttribute.offset = attribute.offset;

            switch (attribute.format)
            {
                case VertexFormat::Float2:
                    nativeAttribute.componentCount = 2;
                    nativeAttribute.type = GL_FLOAT;
                    nativeAttribute.normalized = GL_FALSE;
                    break;

                case VertexFormat::Float3:
                    nativeAttribute.componentCount = 3;
                    nativeAttribute.type = GL_FLOAT;
                    nativeAttribute.normalized = GL_FALSE;
                    break;

                case VertexFormat::Float4:
                    nativeAttribute.componentCount = 4;
                    nativeAttribute.type = GL_FLOAT;
                    nativeAttribute.normalized = GL_FALSE;
                    break;

                case VertexFormat::UByte4:
                    nativeAttribute.componentCount = 4;
                    nativeAttribute.type = GL_UNSIGNED_BYTE;
                    nativeAttribute.normalized = GL_FALSE;
                    break;

                case VertexFormat::UNormByte4:
                    nativeAttribute.componentCount = 4;
                    nativeAttribute.type = GL_UNSIGNED_BYTE;
                    nativeAttribute.normalized = GL_TRUE;
                    break;

                default:
                    Log(LogLevel::Error, "Unsupported vertex format");
                    return {};
            }

            pipeline.attributes.push_back(nativeAttribute);
        }

        PipelineHandle handle;
        handle.id = _nextPipelineHandle++;
        
        _pipelines.emplace(handle.id, pipeline);

        return handle;
    }

    void OpenGLBackend::DestroyPipeline(PipelineHandle handle)
    {
        const auto it = _pipelines.find(handle.id);

        if (it == _pipelines.end())
        {
            Log(LogLevel::Error, "Invalid pipeline handle");
            return;
        }

        if (_currentPipeline.id == handle.id)
        {
            for (const NativePipeline::NativeVertexAttribute& attribute : it->second.attributes)
            {
                glDisableVertexAttribArray(attribute.location);
            }

            _currentPipeline = {};
            glUseProgram(0);
        }

        _pipelines.erase(it);
    }

    TextureHandle OpenGLBackend::CreateTexture(const TextureDesc& desc)
    {
        GLuint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                        desc.width, desc.height, 0,
                        GL_RGBA, GL_UNSIGNED_BYTE, desc.data);

        NativeTexture nativeTexture { static_cast<unsigned int>(texture) };

        TextureHandle handle{};
        handle.id = _nextTextureHandle++;

        _textures.emplace(handle.id, nativeTexture);
        return handle;
    }

    void OpenGLBackend::DestroyTexture(TextureHandle handle)
    {
        const auto it = _textures.find(handle.id);

        if (it == _textures.end())
            return;

        glDeleteTextures(1, &it->second.id);
        _textures.erase(it);
    }

    void OpenGLBackend::SetPipeline(PipelineHandle handle)
    {
        const auto it = _pipelines.find(handle.id);

        if (it == _pipelines.end())
        {
            Log(LogLevel::Error, "Invalid pipeline handle");
            return;
        }

        const auto currentIt = _pipelines.find(_currentPipeline.id);

        if (currentIt != _pipelines.end())
        {
            for (const NativePipeline::NativeVertexAttribute& attribute : currentIt->second.attributes)
            {
                glDisableVertexAttribArray(attribute.location);
            }
        }

        const NativePipeline& pipeline = it->second;

        _currentPipeline = handle;
        glUseProgram(pipeline.program);

        if (pipeline.depthStencilState.depthTestEnabled)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        glDepthMask(pipeline.depthStencilState.depthWriteEnabled ? GL_TRUE : GL_FALSE);

        const RasterizerState& rasterizerState = pipeline.rasterizerState;
        
        glPolygonMode(GL_FRONT_AND_BACK, rasterizerState.fillMode == FillMode::Solid ? GL_FILL : GL_LINE);
        glCullFace(rasterizerState.cullMode == CullMode::Front ? GL_FRONT : GL_BACK);

        if (rasterizerState.cullMode == CullMode::None)
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
        }
    }

    void OpenGLBackend::SetVertexBuffer(BufferHandle handle)
    {
        const auto it = _buffers.find(handle.id);

        if (it == _buffers.end())
        {
            Log(LogLevel::Error, "Invalid buffer handle");
            return;
        }

        if (it->second.type != BufferType::Vertex)
        {
            Log(LogLevel::Error, "Cannot set a non-vertex buffer as vertex buffer");
            return;
        }

        glBindBuffer(GL_ARRAY_BUFFER, it->second.id);
    }

    void OpenGLBackend::SetIndexBuffer(BufferHandle handle)
    {
        const auto it = _buffers.find(handle.id);

        if (it == _buffers.end())
        {
            Log(LogLevel::Error, "Invalid buffer handle");
            return;
        }

        if (it->second.type != BufferType::Index)
        {
            Log(LogLevel::Error, "Cannot set a non-index buffer as index buffer");
            return;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->second.id);
    }

    void OpenGLBackend::SetTexture(TextureHandle handle)
    {
         const auto it = _textures.find(handle.id);

        if (it == _textures.end())
            return;

        glBindTexture(GL_TEXTURE_2D, it->second.id);
    }

    void OpenGLBackend::Draw(unsigned int vertexCount)
    {
        const auto pipelineIt = _pipelines.find(_currentPipeline.id);

        if (pipelineIt == _pipelines.end())
        {
            Log(LogLevel::Error, "No pipeline is currently set");
            return;
        }

        for (const NativePipeline::NativeVertexAttribute& attribute : pipelineIt->second.attributes)
        {
            glEnableVertexAttribArray(attribute.location);

            glVertexAttribPointer(attribute.location, attribute.componentCount, attribute.type, attribute.normalized,
                                  static_cast<GLsizei>(pipelineIt->second.vertexStride), 
                                  reinterpret_cast<const void*>(attribute.offset));
        }

        GLenum topology = GL_TRIANGLES;

        switch (pipelineIt->second.topology)
        {
            case PrimitiveTopology::TriangleList:
                topology = GL_TRIANGLES;
                break;
        }

        glDrawArrays(topology, 0, static_cast<GLsizei>(vertexCount));
    }

    void OpenGLBackend::DrawIndexed(unsigned int indexCount)
    {
        const auto pipelineIt = _pipelines.find(_currentPipeline.id);

        if (pipelineIt == _pipelines.end())
        {
            Log(LogLevel::Error, "No pipeline is currently set");
            return;
        }

        for (const NativePipeline::NativeVertexAttribute& attribute : pipelineIt->second.attributes)
        {
            glEnableVertexAttribArray(attribute.location);

            glVertexAttribPointer(attribute.location, attribute.componentCount, attribute.type, attribute.normalized,
                                  static_cast<GLsizei>(pipelineIt->second.vertexStride),
                                  reinterpret_cast<const void*>(attribute.offset));
        }

        GLenum topology = GL_TRIANGLES;

        switch (pipelineIt->second.topology)
        {
            case PrimitiveTopology::TriangleList:
                topology = GL_TRIANGLES;
                break;
        }

        glDrawElements(topology, static_cast<GLsizei>(indexCount), GL_UNSIGNED_SHORT, nullptr);
    }

    ShaderParameterHandle OpenGLBackend::GetShaderParameter(const ShaderParameter& desc)
    {
        const auto shaderIt = _shaders.find(desc.shader.id);
        if (shaderIt == _shaders.end())
        {
            Log(LogLevel::Error, "Invalid shader handle");
            return {};
        }

        const GLint location = glGetUniformLocation(shaderIt->second.program, desc.name);

        if (location == -1)
        {
            const std::string message = "Shader parameter not found: " + std::string(desc.name);
            Log(LogLevel::Error, message.c_str());
            return {};
        }

        NativeShaderParameter parameter;
        parameter.location = location;
        parameter.type = desc.type;

        ShaderParameterHandle handle;
        handle.id = _nextShaderParameterHandle++;

        _shaderParameters.emplace(handle.id, parameter);

        return handle;
    }

    void OpenGLBackend::DestroyShaderParameter(ShaderParameterHandle handle)
    {
        const auto it = _shaderParameters.find(handle.id);

        if (it == _shaderParameters.end())
        {
            Log(LogLevel::Error, "Invalid shader parameter handle");
            return;
        }

        _shaderParameters.erase(it);
    }

    void OpenGLBackend::SetShaderParameter(ShaderParameterHandle handle, const void* data, unsigned int count)
    {
        const auto it = _shaderParameters.find(handle.id);

        if (it == _shaderParameters.end())
        {
            Log(LogLevel::Error, "Invalid shader parameter handle");
            return;
        }

        const NativeShaderParameter& parameter = it->second;

        switch (parameter.type)
        {
            case ShaderParameterType::Int:
                glUniform1iv(parameter.location, static_cast<GLsizei>(count), static_cast<const GLint*>(data));
                break;

            case ShaderParameterType::Int2:
                glUniform2iv(parameter.location, static_cast<GLsizei>(count), static_cast<const GLint*>(data));
                break;

            case ShaderParameterType::Int3:
                glUniform3iv(parameter.location, static_cast<GLsizei>(count), static_cast<const GLint*>(data));
                break;

            case ShaderParameterType::Int4:
                glUniform4iv(parameter.location, static_cast<GLsizei>(count), static_cast<const GLint*>(data));
                break;

            case ShaderParameterType::Float:
                glUniform1fv(parameter.location, static_cast<GLsizei>(count), static_cast<const GLfloat*>(data));
                break;

            case ShaderParameterType::Float2:
                glUniform2fv(parameter.location, static_cast<GLsizei>(count), static_cast<const GLfloat*>(data));
                break;

            case ShaderParameterType::Float3:
                glUniform3fv(parameter.location, static_cast<GLsizei>(count), static_cast<const GLfloat*>(data));
                break;

            case ShaderParameterType::Float4:
                glUniform4fv(parameter.location, static_cast<GLsizei>(count), static_cast<const GLfloat*>(data));
                break;

            case ShaderParameterType::Mat2:
                glUniformMatrix2fv(parameter.location, static_cast<GLsizei>(count), GL_FALSE, static_cast<const GLfloat*>(data));
                break;

            case ShaderParameterType::Mat3:
                glUniformMatrix3fv(parameter.location, static_cast<GLsizei>(count), GL_FALSE, static_cast<const GLfloat*>(data));
                break;

            case ShaderParameterType::Mat4:
                glUniformMatrix4fv(parameter.location, static_cast<GLsizei>(count), GL_FALSE, static_cast<const GLfloat*>(data));
                break;
        }
    }
}
