#pragma once

#include "../internal/backend.h"

namespace dvl::internal
{
    class VitaGLBackend : public Backend
    {
    public:
        bool Initialize(const DeviceDesc& desc) override;
        void Shutdown() override;

        void BeginFrame(const Color& clearColor) override;
        void EndFrame() override;

        void SetViewport(const Viewport& viewport) override;
    };
}
