#pragma once

#include <entityx/Entity.h>
#include <entityx/System.h>

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/System.hh>
#include <conflagrant/SystemFactory.hh>

namespace cfl {
namespace syst {
class EcsDebugger : public System, public entityx::System<EcsDebugger> {
    std::unordered_set<std::shared_ptr<SystemFactory>> currentSystems;

    entityx::Entity currentEntity;

    void DrawEntityEditor(entityx::Entity &entity);

    void
    DrawSystems(std::unordered_set<std::shared_ptr<SystemFactory>> &currentSystems, entityx::SystemManager &manager);


public:
    inline static string GetName() {
        $
        return "EcsDebugger";
    }

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, EcsDebugger &sys) {
        $
        json["name"] = GetName();
        return true;
    }

    static bool DrawWithImGui(EcsDebugger &sys, InputManager const &input) {
        $
        return false;
    }
};
} // namespace syst
} // namespace cfl
