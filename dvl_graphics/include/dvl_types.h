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

    struct Color
    {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 1.0f;
    };
}
