#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/System.hh>
#include <conflagrant/SystemFactory.hh>
#include <conflagrant/gl/Shader.hh>
#include <conflagrant/gl/Mesh.hh>
#include <conflagrant/serialization/serialize.hh>

#include <entityx/Entity.h>
#include <entityx/System.h>

namespace cfl {
namespace syst {
class EcsDebugger : public System, public entityx::System<EcsDebugger> {
public:
    static constexpr auto SystemName = "EcsDebugger";

private:
    std::unordered_set<std::shared_ptr<SystemFactory>> currentSystems;

    entityx::Entity currentEntity;

    void DrawEntityEditor(entityx::Entity &entity);

    void
    DrawSystems(std::unordered_set<std::shared_ptr<SystemFactory>> &currentSystems, entityx::SystemManager &manager);

    void LoadShaders();

public:
    EcsDebugger();

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    inline static bool Serialize(BaseSerializer const& serializer, Json::Value &json,
                          EcsDebugger &sys) {
        $
        json["name"] = SystemName;
        return true;
    }
};
} // namespace syst
} // namespace cfl
