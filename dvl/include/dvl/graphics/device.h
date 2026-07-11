#pragma once

#include "types.h"

#include <memory>

namespace dvl::internal
{
    class Backend;
}

namespace dvl
{
    class Device
    {
    public:
        Device();
        ~Device();

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        bool Initialize(const DeviceDesc& desc);
        void Shutdown();

        void BeginFrame(const Color& clearColor);
        void EndFrame();

        bool IsInitialized() const;

        void SetViewport(const Viewport& viewport);

    private:
        std::unique_ptr<internal::Backend> _backend;
    };
}
