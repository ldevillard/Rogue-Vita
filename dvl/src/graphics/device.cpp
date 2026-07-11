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
}
