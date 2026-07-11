#include <dvl/graphics/device.h>
#include <dvl/log/log.h>

#include "internal/backend.h"
#include "internal/backend_factory.h"

#include <utility>

namespace dvl
{
    Device::Device() = default;

    Device::~Device()
    {
        Shutdown();
    }

    bool Device::Initialize(const DeviceDesc& desc)
    {
        if (_backend)
        {
            Log(LogLevel::Error, "Graphics device is already initialized");
            return false;
        }

        if (desc.width <= 0 || desc.height <= 0)
        {
            Log(LogLevel::Error, "Invalid graphics device dimensions");
            return false;
        }

        std::unique_ptr<internal::Backend> backend = internal::CreateBackend(desc.api);

        if (!backend)
        {
            Log(LogLevel::Error, "Failed to create graphics backend");
            return false;
        }

        if (!backend->Initialize(desc))
        {
            Log(LogLevel::Error, "Failed to initialize graphics backend");
            return false;
        }

        _backend = std::move(backend);
        
        return true;
    }

    void Device::Shutdown()
    {
        if (!_backend)
            return;

        _backend->Shutdown();
        _backend.reset();
    }

    void Device::BeginFrame(const Color& clearColor)
    {
        if (!_backend)
            return;

        _backend->BeginFrame(clearColor);
    }

    void Device::EndFrame()
    {
        if (!_backend)
            return;

        _backend->EndFrame();
    }

    bool Device::IsInitialized() const
    {
        return _backend != nullptr;
    }

    void Device::SetViewport(const Viewport& viewport)
    {
        if (!_backend)
        {
            Log(LogLevel::Error, "Cannot set viewport before device initialization");
            return;
        }

        if (viewport.width <= 0 || viewport.height <= 0)
        {
            Log(LogLevel::Error, "Invalid viewport dimensions");
            return;
        }
        
        _backend->SetViewport(viewport);
    }

    BufferHandle Device::CreateBuffer(const BufferDesc& desc)
    {
        if (!_backend)
        {
            Log(LogLevel::Error, "Cannot create buffer before device initialization");
            return {};
        }

        if (desc.size == 0)
        {
            Log(LogLevel::Error, "Invalid buffer description");
            return {};
        }

        if (desc.usage == BufferUsage::Static && desc.data == nullptr)
        {
            Log(LogLevel::Error, "Static buffer requires initial data");
            return {};
        }
        
        return _backend->CreateBuffer(desc);
    }

    void Device::DestroyBuffer(BufferHandle handle)
    {
        if (!_backend || !handle.IsValid())
        {
            return;
        }
    
        _backend->DestroyBuffer(handle);
    }

    ShaderHandle Device::CreateShader(const ShaderDesc& desc)
    {
        if (!_backend)
        {
            Log(LogLevel::Error, "Cannot create shader before device initialization");
            return {};
        }

        if (desc.vertex.data == nullptr || desc.vertex.size == 0 ||
            desc.fragment.data == nullptr || desc.fragment.size == 0)
        {
            Log(LogLevel::Error, "Invalid shader description");
            return {};
        }

        return _backend->CreateShader(desc);
    }

    void Device::DestroyShader(ShaderHandle handle)
    {
        if (!_backend || !handle.IsValid())
        {
            return;
        }

        _backend->DestroyShader(handle);
    }

    PipelineHandle Device::CreatePipeline(const PipelineDesc& desc)
    {
        if (!_backend)
        {
            Log(LogLevel::Error, "Cannot create pipeline before device initialization");
            return {};
        }

        return _backend->CreatePipeline(desc);
    }

    void Device::DestroyPipeline(PipelineHandle handle)
    {
        if (!_backend || !handle.IsValid())
        {
            return;
        }

        _backend->DestroyPipeline(handle);
    }

    void Device::SetPipeline(PipelineHandle handle)
    {
        if (!_backend || !handle.IsValid())
        {
            return;
        }

        _backend->SetPipeline(handle);
    }

    void Device::SetVertexBuffer(BufferHandle handle)
    {
        if (!_backend || !handle.IsValid())
        {
            return;
        }

        _backend->SetVertexBuffer(handle);
    }

    void Device::Draw(unsigned int vertexCount)
    {
        if (!_backend)
        {
            return;
        }

        _backend->Draw(vertexCount);
    }
}
