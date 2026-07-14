#pragma once

#include <dvl/dvl.h>

#include <glm/glm.hpp>

struct Mesh;
struct MeshDesc;
struct Material;
struct RenderPipeline;
struct RenderPipelineDesc;

class Camera;
class DirectionalLight;
class Transform;

class Renderer
{
public:
    Renderer(int screenWidth, int screenHeight);
    ~Renderer();

    bool CreateMesh(const MeshDesc& desc, Mesh& mesh);
    void DestroyMesh(Mesh& mesh);

    RenderPipeline CreateRenderPipeline(const RenderPipelineDesc& desc);
    void DestroyRenderPipeline(RenderPipeline& renderPipeline);

    void BeginFrame(const dvl::Color& clearColor);
    void EndFrame();

    void BeginScene(const Camera& camera);
    void SubmitLight(const DirectionalLight& light);

    void Draw(const Mesh& mesh, const Material& material, const Transform& transform);

private:
    static constexpr int MAX_LIGHTS = 4;

    dvl::Device _device;
    const Camera* _activeCamera = nullptr;
    
    glm::vec4 _lightDirections[MAX_LIGHTS]{};
    glm::vec4 _lightColors[MAX_LIGHTS]{};

    int _lightCount = 0;
};
