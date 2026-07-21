#pragma once

namespace dvl
{
    enum class GraphicsAPI
    {
        VitaGL
    };

    struct DeviceDesc
    {
        GraphicsAPI api = GraphicsAPI::VitaGL;

        int width = 960;
        int height = 544;
    
        bool vsync = false;
    };

    struct Viewport
    {
        int x = 0;
        int y = 0;
        int width = 960;
        int height = 544;
    };
}
