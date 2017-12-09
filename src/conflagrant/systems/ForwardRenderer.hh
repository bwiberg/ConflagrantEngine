#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/System.hh>
#include <conflagrant/gl/Shader.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/RenderStats.hh>

#include <entityx/System.h>

namespace cfl {
namespace syst {
class ForwardRenderer : public cfl::System, public entityx::System<ForwardRenderer> {
public:
    static constexpr auto SystemName = "ForwardRenderer";

private:
    std::shared_ptr<gl::Shader>
            forwardShader,
            skydomeShader,
            shadowmapLightpassShader,
            shadowmapVisShader,
            wireframeShader;

    RenderStats renderStats;

    bool cullModelsAndMeshes{false}, renderBoundingSpheres{false}, renderBoundingSpheresAsWireframe{true};

    void LoadShaders();

public:
    ForwardRenderer();

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 ForwardRenderer &sys) {
        $
        json["name"] = SystemName;
        return true;
    }

    static bool DrawWithImGui(ForwardRenderer &sys, InputManager const &input);
};
} // namespace syst
} // namespace cfl
