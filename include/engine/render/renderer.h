#pragma once

#include <dvl/dvl.h>

#include <glm/glm.hpp>

struct Mesh;
struct MeshDesc;
struct Material;
struct RenderPipeline;
struct RenderPipelineDesc;

class AssetRegistry;
class Camera;
class DirectionalLight;
class Transform;

class Renderer
{
public:
    // TODO: Create a RenderSystem that will resolve asset handles
    Renderer(int screenWidth, int screenHeight, const AssetRegistry& assetRegistry);
    ~Renderer();

    bool CreateMesh(const MeshDesc& desc, Mesh& mesh);
    void DestroyMesh(Mesh& mesh);

    RenderPipeline CreateRenderPipeline(const RenderPipelineDesc& desc);
    void DestroyRenderPipeline(RenderPipeline& renderPipeline);

    // TODO: Create an engine side texture desc and handle
    dvl::TextureHandle CreateTexture(const dvl::TextureDesc& desc);
    void DestroyTexture(dvl::TextureHandle texture);

    void BeginFrame(const dvl::Color& clearColor);
    void EndFrame();

    void BeginScene(const Camera& camera);
    void SubmitLight(const DirectionalLight& light);

    void Draw(const Mesh& mesh, const Material& material, const Transform& transform);

private:
    void setParameter(const RenderPipeline& renderPipeline, const char* name,
                      const void* data, unsigned int count = 1);

    static constexpr int MaxLights = 4;

    dvl::Device _device;
    const Camera* _activeCamera = nullptr;
    const AssetRegistry& _assetRegistry;

    glm::vec4 _lightDirections[MaxLights]{};
    glm::vec4 _lightColors[MaxLights]{};

    int _lightCount = 0;
};
