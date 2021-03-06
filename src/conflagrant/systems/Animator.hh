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
class Animator : public System, public entityx::System<Animator> {
public:
    static constexpr auto SystemName = "Animator";

private:
    bool startAllNextFrame{false}, stopAllNextFrame{false};

public:
    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 Animator &sys) {
        json["name"] = SystemName;
        return true;
    }

    static bool DrawWithImGui(Animator &sys, InputManager const &input);
};
} // namespace syst
} // namespace cfl
