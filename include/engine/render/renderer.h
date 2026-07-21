#pragma once

#include <dvl/dvl.h>

#include <glm/glm.hpp>

struct Mesh;
struct MeshDesc;
struct Material;
struct RenderPipeline;
struct RenderPipelineDesc;
struct Texture;
struct TextureDesc;

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

    bool CreateRenderPipeline(const RenderPipelineDesc& desc, RenderPipeline& renderPipeline);
    void DestroyRenderPipeline(RenderPipeline& renderPipeline);

    bool CreateTexture(const TextureDesc& desc, Texture& texture);
    void DestroyTexture(Texture& texture);

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
