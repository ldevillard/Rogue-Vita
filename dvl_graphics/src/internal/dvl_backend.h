#pragma once 

#include <dvl_types.h>

namespace dvl::internal
{
    class Backend
    {
    public:
        virtual ~Backend() = default;

        virtual bool Initialize(const DeviceDesc& desc) = 0;
        virtual void Shutdown() = 0;

        virtual void BeginFrame(const Color& clearColor) = 0;
        virtual void EndFrame() = 0;
    };
}