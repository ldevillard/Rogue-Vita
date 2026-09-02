#pragma once

#include <dvl/dvl.h>

#include <cstdint>

struct Mesh;
struct MeshDesc;
struct Material;
struct RenderPipeline;
struct RenderPipelineDesc;
struct ShaderParameterBinding;
struct Texture;
struct TextureDesc;

class AssetRegistry;
class Camera;
class DirectionalLight;
class Transform;

struct DrawContext
{
    const Camera* camera = nullptr;
    const Material* material = nullptr;

    const dvl::Mat4* modelMatrix = nullptr;

    const dvl::Mat4* skinningMatrices = nullptr;
    std::uint32_t boneCount = 0;
};

class Renderer
{
public:
    // TODO: Create a RenderSystem that will resolve asset handles
    Renderer(int screenWidth, int screenHeight, const AssetRegistry& assetRegistry);
    ~Renderer();

    bool CreateMesh(const MeshDesc& desc, Mesh& mesh);
    bool UpdateMeshVertices(const Mesh& mesh, const void* vertexData, std::size_t vertexDataSize);
    void DestroyMesh(Mesh& mesh);

    bool CreateRenderPipeline(const RenderPipelineDesc& desc, RenderPipeline& renderPipeline);
    void DestroyRenderPipeline(RenderPipeline& renderPipeline);

    bool CreateTexture(const TextureDesc& desc, Texture& texture);
    void DestroyTexture(Texture& texture);

    void BeginFrame(const dvl::Vec4& clearColor);
    void EndFrame();

    bool ShouldClose() const;

    void BeginScene(const Camera& camera);
    void SubmitLight(const DirectionalLight& light);

    void Draw(const Mesh& mesh, const Material& material, const dvl::Mat4& modelMatrix);
    void DrawSkinned(const Mesh& mesh, const Material& material, const dvl::Mat4& modelMatrix, const dvl::Mat4* skinningMatrices, int boneCount);

private:
    void bindParameter(const ShaderParameterBinding& parameter, const DrawContext& context);

    static constexpr int MaxLights = 4;
    static constexpr int MaxBones = 32;

    dvl::Device _device;
    const Camera* _activeCamera = nullptr;
    const AssetRegistry& _assetRegistry;

    dvl::Vec4 _lightDirections[MaxLights]{};
    dvl::Vec4 _lightColors[MaxLights]{};

    int _lightCount = 0;
};
