#pragma once

#include <entityx/System.h>

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/System.hh>

#include <conflagrant/gl/Shader.hh>

namespace cfl {
namespace syst {
class ForwardRenderer : public cfl::System, public entityx::System<ForwardRenderer> {
    std::shared_ptr<gl::Shader> forwardShader;

public:
    ForwardRenderer();

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    inline static string GetName() {
        return "ForwardRenderer";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, ForwardRenderer &sys) {
        json["name"] = GetName();
        return true;
    }

    static bool DrawWithImGui(ForwardRenderer &sys, InputManager const &input) {
        return true;
    }
};
} // namespace syst
} // namespace cfl
