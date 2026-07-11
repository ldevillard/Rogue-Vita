#include <dvl/dvl.h>

#include "renderer/shader_loader.h"
#include "renderer/vertex.h"

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

	// Test of a simple triangle vertex buffer creation
	{
		VertexPositionColor vertices[3] = 
		{
			{ -0.5f, -0.5f, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{  0.5f, -0.5f, 0.0f, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{  0.0f,  0.5f, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		dvl::BufferDesc bufferDesc;
		bufferDesc.type = dvl::BufferType::Vertex;
		bufferDesc.usage = dvl::BufferUsage::Static;
		bufferDesc.size = sizeof(vertices);
		bufferDesc.data = vertices;

		dvl::BufferHandle vertexBuffer = device.CreateBuffer(bufferDesc);

		if (!vertexBuffer.IsValid())
		{
			dvl::Log(dvl::LogLevel::Error, "Failed to create vertex buffer");
			return 1;
		}
	}

	// Test of a simple shader creation
	{
		ShaderSource shaderSource;
		if (!LoadShaderSource("app0:/assets/shaders/vertex.vert", "app0:/assets/shaders/fragment.frag", shaderSource))
		{
			dvl::Log(dvl::LogLevel::Error, "Failed to load shader source");
			return 1;
		}

		dvl::ShaderDesc shaderDesc;
		shaderDesc.vertex.data = shaderSource.vertex.c_str();
		shaderDesc.vertex.size = shaderSource.vertex.size();
		shaderDesc.fragment.data = shaderSource.fragment.c_str();
		shaderDesc.fragment.size = shaderSource.fragment.size();

		dvl::ShaderHandle shaderHandle = device.CreateShader(shaderDesc);

		if (!shaderHandle.IsValid())
		{
			dvl::Log(dvl::LogLevel::Error, "Failed to create shader");
			return 1;
		}
	}

	while (true)
	{
		device.BeginFrame({ 0.118f, 0.353f, 0.706f, 1.0f });
		
		device.EndFrame();
	}

    return 0;
}
