#pragma once

#include <entityx/System.h>

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/System.hh>

namespace cfl {
namespace syst {
struct ForwardRenderer : public cfl::System, public entityx::System<ForwardRenderer> {
    ForwardRenderer();

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    inline static string GetName() {
        return "ForwardRenderer";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, ForwardRenderer &system) {
        return true;
    }
};
} // namespace syst
} // namespace cfl
