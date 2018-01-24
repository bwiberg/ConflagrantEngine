#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/System.hh>
#include <conflagrant/serialization/serialize.hh>

#include <entityx/System.h>

namespace cfl {
namespace syst {
class ProceduralGenerator : public System, public entityx::System<ProceduralGenerator> {
public:
    static constexpr auto SystemName = "ProceduralGenerator";

private:


public:
    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 ProceduralGenerator &sys) {
        json["name"] = SystemName;
        return true;
    }

    static bool DrawWithImGui(ProceduralGenerator &sys, InputManager const &input);
};
} // namespace syst
} // namespace cfl
