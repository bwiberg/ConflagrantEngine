#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/System.hh>
#include <conflagrant/gl/Shader.hh>
#include <conflagrant/gl/Framebuffer.hh>
#include <conflagrant/gl/Renderbuffer.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/RenderStats.hh>

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

    std::shared_ptr<gl::Framebuffer> framebuffer;
    std::shared_ptr<gl::Texture2D> positionTexture, normalShininessTexture, albedoSpecularTexture;
    std::shared_ptr<gl::Renderbuffer> depthRenderbuffer;

    RenderStats renderStats;
    bool cullModelsAndMeshes{true}, renderBoundingSpheres{false}, renderBoundingSpheresAsWireframe{true};

    uvec2 lastWindowSize{0, 0};

    bool UpdateFramebuffer(GLsizei const width, GLsizei const height);

    void LoadShaders();

public:
    DeferredRenderer();

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 DeferredRenderer &sys) {
        json["name"] = SystemName;
        return true;
    }

    static bool DrawWithImGui(DeferredRenderer &sys, InputManager const &input);
};
} // namespace syst
} // namespace cfl
