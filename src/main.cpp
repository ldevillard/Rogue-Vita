#include <dvl.h>

#include <iostream>

#include <psp2/kernel/clib.h>

int main()
{
	dvl::Device device;
	dvl::DeviceDesc desc;
	desc.api = dvl::GraphicsAPI::VitaGL;
	desc.width = 960;
	desc.height = 544;
	desc.vsync = true;

	if (!device.Initialize(desc))
	{
		return 1;
	}

	while (true)
	{
		device.BeginFrame({ 0.118f, 0.353f, 0.706f, 1.0f });
		
		device.EndFrame();
	}

    return 0;
}
