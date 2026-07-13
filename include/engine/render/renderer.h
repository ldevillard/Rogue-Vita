#pragma once

#include <dvl/dvl.h>

struct Mesh;
struct MeshDesc;
struct Material;
struct RenderPipeline;
struct RenderPipelineDesc;

class Camera;
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

    void Draw(const Mesh& mesh, 
                const Material& material, 
                const Transform& transform);

private:
    dvl::Device _device;
    const Camera* _activeCamera = nullptr;
};
