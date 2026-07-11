#include <dvl/dvl.h>

int main()
{
	dvl::Log(dvl::LogLevel::Info, "Application starting");

	dvl::Device device;
	dvl::DeviceDesc desc;
	desc.api = dvl::GraphicsAPI::VitaGL;
	desc.width = 960;
	desc.height = 544;
	desc.vsync = true;

	dvl::Log(dvl::LogLevel::Info, "Initializing graphics device");

	if (!device.Initialize(desc))
	{
		dvl::Log(dvl::LogLevel::Error, "Failed to initialize graphics device");
		return 1;
	}

	dvl::Log(dvl::LogLevel::Info, "Graphics device initialized");

	while (true)
	{
		device.BeginFrame({ 0.118f, 0.353f, 0.706f, 1.0f });
		
		device.EndFrame();
	}

    return 0;
}
