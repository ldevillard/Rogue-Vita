#include <dvl_device.h>

#include "../internal/dvl_backend.h"
#include "../internal/dvl_backend_factory.h"

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
        if (_backend || desc.width <= 0 || desc.height <= 0)
            return false;

        std::unique_ptr<internal::Backend> backend = internal::CreateBackend(desc.api);
        
        if (!backend || !backend->Initialize(desc))
            return false;

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
