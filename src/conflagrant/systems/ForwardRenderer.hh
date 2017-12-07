#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/System.hh>
#include <conflagrant/gl/Shader.hh>
#include <conflagrant/serialization/serialize.hh>

#include <entityx/System.h>

namespace cfl {
namespace syst {
struct RenderStats {
    size_t numVertices{0};
    size_t numTriangles{0};
    size_t numMeshes{0};
    size_t numPointLights{0};
    size_t numDirectionalLights{0};
};

class ForwardRenderer : public cfl::System, public entityx::System<ForwardRenderer> {
public:
    static constexpr auto SystemName = "ForwardRenderer";

private:
    std::shared_ptr<gl::Shader> forwardShader, skydomeShader, shadowmapLightpassShader, shadowmapVisShader;

    RenderStats renderStats;

    void LoadShaders();

public:
    ForwardRenderer();

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    inline static bool Serialize(BaseSerializer const& serializer, Json::Value &json,
                                 ForwardRenderer &sys) {
        $
        json["name"] = SystemName;
        return true;
    }

    static bool DrawWithImGui(ForwardRenderer &sys, InputManager const &input);
};
} // namespace syst
} // namespace cfl
