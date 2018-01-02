#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/System.hh>
#include <conflagrant/gl/Shader.hh>
#include <conflagrant/gl/Framebuffer.hh>
#include <conflagrant/gl/Renderbuffer.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/RenderStats.hh>
#include <conflagrant/Time.hh>

#ifdef ENABLE_VOXEL_CONE_TRACING
#include <conflagrant/components/OrthographicCamera.hh>
#include <conflagrant/components/Transform.hh>
#include <conflagrant/math.hh>
#endif // ENABLE_VOXEL_CONE_TRACING

#include <entityx/System.h>

namespace cfl {
namespace syst {
class DeferredRenderer : public System, public entityx::System<DeferredRenderer> {
public:
    static constexpr auto SystemName = "DeferredRenderer";

private:
    std::shared_ptr<gl::Shader>
            geometryShader,
            directionalLightShadowShader,
            lightsShader,
            skydomeShader,
            wireframeShader;

    std::map<string, double> durationsByName;
    std::list<string> durationsOrder;

#ifdef ENABLE_VOXEL_CONE_TRACING
    std::shared_ptr<gl::Shader>
            voxelizeShader,
            voxelDirectRenderingShader,
            mipmapShader,
            voxelConeTracingShader;

    bool useVoxelConeTracing{true};
    struct {
        GLsizei textureDimensionExponent{7}; // (2^7)^3
        float halfDimensions{1};
        vec3 center{0, 0, 0};
        int mipmapLevels{7};
        float timeBetweenMipmapGeneration{0.1f};
        cfl::time_t timeOfLastMipmapGeneration{std::numeric_limits<cfl::time_t>::min()};
        bool useComputeShaderMipmapper{true};

        bool useDirectLighting{true}, useIndirectDiffuseLighting{true}, useIndirectSpecularLighting{true};

        bool useDirectVoxelRendering{true};
        struct {
            int mipmapLevel{0}, raymarchingSteps{32};
            float renderDistance{25.0f};
        } DirectRendering;
    } VCT;

    inline GLsizei GetActualVoxelTextureSize() const {
        return static_cast<GLsizei>(math::Pow(2, VCT.textureDimensionExponent));
    }

    std::shared_ptr<gl::Texture3D> voxelTexture;
#endif // ENABLE_VOXEL_CONE_TRACING

    std::shared_ptr<gl::Framebuffer> framebuffer;
    std::shared_ptr<gl::Texture2D> positionRadianceTexture, normalShininessTexture, albedoSpecularTexture;
    std::shared_ptr<gl::Renderbuffer> depthRenderbuffer;

    RenderStats renderStats;
    bool cullModelsAndMeshes{true}, renderBoundingSpheres{false}, renderBoundingSpheresAsWireframe{true};

    uvec2 lastWindowSize{0, 0};

    bool UpdateFramebuffer(GLsizei const width, GLsizei const height);

    void LoadShaders();

public:
    DeferredRenderer();

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 DeferredRenderer &sys);

    static bool DrawWithImGui(DeferredRenderer &sys, InputManager const &input);
};
} // namespace syst
} // namespace cfl
