#include <dvl/dvl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

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

	// -- BUFFER --
	const dvl::Color bottomLeftBack = {0.0f, 0.0f, 0.0f, 1.0f};
	const dvl::Color bottomRightBack = {1.0f, 0.0f, 0.0f, 1.0f};
	const dvl::Color topLeftBack = {0.0f, 1.0f, 0.0f, 1.0f};
	const dvl::Color topRightBack = {1.0f, 1.0f, 0.0f, 1.0f};
	const dvl::Color bottomLeftFront = {0.0f, 0.0f, 1.0f, 1.0f};
	const dvl::Color bottomRightFront = {1.0f, 0.0f, 1.0f, 1.0f};
	const dvl::Color topLeftFront = {0.0f, 1.0f, 1.0f, 1.0f};
	const dvl::Color topRightFront = {1.0f, 1.0f, 1.0f, 1.0f};

	VertexPositionColor vertices[] = 
	{
		// Front face
		{-0.5f, -0.5f,  0.5f, bottomLeftFront},
		{ 0.5f, -0.5f,  0.5f, bottomRightFront},
		{ 0.5f,  0.5f,  0.5f, topRightFront},

		{-0.5f, -0.5f,  0.5f, bottomLeftFront},
		{ 0.5f,  0.5f,  0.5f, topRightFront},
		{-0.5f,  0.5f,  0.5f, topLeftFront},

		// Back face
		{ 0.5f, -0.5f, -0.5f, bottomRightBack},
		{-0.5f, -0.5f, -0.5f, bottomLeftBack},
		{-0.5f,  0.5f, -0.5f, topLeftBack},

		{ 0.5f, -0.5f, -0.5f, bottomRightBack},
		{-0.5f,  0.5f, -0.5f, topLeftBack},
		{ 0.5f,  0.5f, -0.5f, topRightBack},

		// Left face
		{-0.5f, -0.5f, -0.5f, bottomLeftBack},
		{-0.5f, -0.5f,  0.5f, bottomLeftFront},
		{-0.5f,  0.5f,  0.5f, topLeftFront},

		{-0.5f, -0.5f, -0.5f, bottomLeftBack},
		{-0.5f,  0.5f,  0.5f, topLeftFront},
		{-0.5f,  0.5f, -0.5f, topLeftBack},

		// Right face
		{ 0.5f, -0.5f,  0.5f, bottomRightFront},
		{ 0.5f, -0.5f, -0.5f, bottomRightBack},
		{ 0.5f,  0.5f, -0.5f, topRightBack},

		{ 0.5f, -0.5f,  0.5f, bottomRightFront},
		{ 0.5f,  0.5f, -0.5f, topRightBack},
		{ 0.5f,  0.5f,  0.5f, topRightFront},

		// Top face
		{-0.5f,  0.5f,  0.5f, topLeftFront},
		{ 0.5f,  0.5f,  0.5f, topRightFront},
		{ 0.5f,  0.5f, -0.5f, topRightBack},

		{-0.5f,  0.5f,  0.5f, topLeftFront},
		{ 0.5f,  0.5f, -0.5f, topRightBack},
		{-0.5f,  0.5f, -0.5f, topLeftBack},

		// Bottom face
		{-0.5f, -0.5f, -0.5f, bottomLeftBack},
		{ 0.5f, -0.5f, -0.5f, bottomRightBack},
		{ 0.5f, -0.5f,  0.5f, bottomRightFront},

		{-0.5f, -0.5f, -0.5f, bottomLeftBack},
		{ 0.5f, -0.5f,  0.5f, bottomRightFront},
		{-0.5f, -0.5f,  0.5f, bottomLeftFront}
	};

	unsigned int vertexCount = sizeof(vertices) / sizeof(VertexPositionColor);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), static_cast<float>(desc.width) / static_cast<float>(desc.height), 0.1f, 100.0f);

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

	// -- SHADER --
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

	// -- PIPELINE --
	const dvl::VertexAttribute attributes[] = 
	{
		{ "aPosition", dvl::VertexFormat::Float3, offsetof(VertexPositionColor, x) },
		{ "aColor", dvl::VertexFormat::Float4, offsetof(VertexPositionColor, color) }
	};

	dvl::PipelineDesc pipelineDescSolid;
	pipelineDescSolid.shader = shaderHandle;
	pipelineDescSolid.attributes = attributes;
	pipelineDescSolid.attributeCount = sizeof(attributes) / sizeof(attributes[0]);
	pipelineDescSolid.vertexStride = sizeof(VertexPositionColor);
	pipelineDescSolid.topology = dvl::PrimitiveTopology::TriangleList;
	pipelineDescSolid.depthStencilState.depthTestEnabled = true;
	pipelineDescSolid.depthStencilState.depthWriteEnabled = true;

	dvl::PipelineHandle pipelineHandleSolid = device.CreatePipeline(pipelineDescSolid);
	if (!pipelineHandleSolid.IsValid())
	{
		dvl::Log(dvl::LogLevel::Error, "Failed to create pipeline");
		return 1;
	}

	dvl::PipelineDesc pipelineDescWireframe;
	pipelineDescWireframe.shader = shaderHandle;
	pipelineDescWireframe.attributes = attributes;
	pipelineDescWireframe.attributeCount = sizeof(attributes) / sizeof(attributes[0]);
	pipelineDescWireframe.vertexStride = sizeof(VertexPositionColor);
	pipelineDescWireframe.topology = dvl::PrimitiveTopology::TriangleList;
	pipelineDescWireframe.depthStencilState.depthTestEnabled = true;
	pipelineDescWireframe.depthStencilState.depthWriteEnabled = true;
	pipelineDescWireframe.rasterizerState.fillMode = dvl::FillMode::Wireframe;

	dvl::PipelineHandle pipelineHandleWireframe = device.CreatePipeline(pipelineDescWireframe);
	if (!pipelineHandleWireframe.IsValid())
	{
		dvl::Log(dvl::LogLevel::Error, "Failed to create pipeline");
		return 1;
	}

	// -- SHADER PARAMETER --

	dvl::ShaderParameter shaderParameterDesc;
	shaderParameterDesc.shader = shaderHandle;
	shaderParameterDesc.name = "mvpMatrix";
	shaderParameterDesc.type = dvl::ShaderParameterType::Mat4;
	dvl::ShaderParameterHandle shaderParameterHandle = device.GetShaderParameter(shaderParameterDesc);
	if (!shaderParameterHandle.IsValid())
	{
		dvl::Log(dvl::LogLevel::Error, "Failed to get shader parameter");
		return 1;
	}

	while (true)
	{
		device.BeginFrame({ 0.118f, 0.122f, 0.278f });
		
		device.SetPipeline(pipelineHandleSolid);
		device.SetVertexBuffer(vertexBuffer);

		// rotation of the cube over time
		static float rotationAngle = 0.0f;
		rotationAngle += 0.05f; // Adjust the rotation speed as needed

		glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::mat4 tiltZ = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		const glm::mat4 tiltX = glm::rotate(glm::mat4(1.0f), glm::radians(35.264f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 spinY = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = translation * spinY * tiltX * tiltZ;

		glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
		device.SetShaderParameter(shaderParameterHandle, &mvpMatrix, 1);

		device.Draw(vertexCount);

		translation = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		spinY = glm::rotate(glm::mat4(1.0f), -rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = translation * spinY * tiltX * tiltZ;
		mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;

		device.SetPipeline(pipelineHandleWireframe);

		device.SetShaderParameter(shaderParameterHandle, &mvpMatrix, 1);
		
		device.Draw(vertexCount);

		device.EndFrame();
	}

    return 0;
}
