#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/CL.hh>
#include <conflagrant/System.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/gl/Shader.hh>
#include <conflagrant/RenderStats.hh>
#include <conflagrant/gl/Mesh.hh>
#include <conflagrant/gl/Framebuffer.hh>

#include <entityx/System.h>

namespace cfl {
namespace syst {
class SnowfallAnimator : public System, public entityx::System<SnowfallAnimator> {
public:
    static constexpr auto SystemName = "SnowfallAnimator";

private:
    void LoadShaders();

    void ResetTopDownFramebuffer(GLsizei size);

    void RenderTopDownDepthTexture();

    std::shared_ptr<gl::Shader>
            simulateComputeShader,
            sortByDepthComputeShader;

    std::shared_ptr<gl::Texture2D> heightTexture;
    std::shared_ptr<gl::Framebuffer> heightFramebuffer;
    int heightTexturePower{10};

    RenderStats renderStats;

    bool enabled{true};

    bool sortByDepth{false};

public:
    SnowfallAnimator();

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                          SnowfallAnimator &sys);

    static bool DrawWithImGui(SnowfallAnimator &sys, InputManager const &input);
};
} // namespace syst
} // namespace cfl
