#pragma once

#include <conflagrant/System.hh>
#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/components/ActiveCamera.hh>
#include <conflagrant/components/PerspectiveCamera.hh>
#include <conflagrant/components/OrthographicCamera.hh>
#include <conflagrant/components/Transform.hh>
#include <conflagrant/components/Name.hh>
#include <conflagrant/components/Guid.hh>

#include <conflagrant/events.hh>
#include <entityx/System.h>
#include <conflagrant/Time.hh>

namespace cfl {
namespace syst {
class CameraController : public cfl::System, public entityx::System<CameraController> {
public:
    static constexpr auto SystemName = "CameraController";

private:
    float movementSpeed{3};
    float turnSpeed{0.3};

    bool isActive;

    void MoveCamera(comp::Transform &transform);

public:
    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    inline static bool Serialize(BaseSerializer const& serializer, Json::Value &json,
                                 CameraController &sys) {
        $
        json["name"] = SystemName;
        return true;
    }

    static bool DrawWithImGui(CameraController &sys, InputManager const &input);
};
} // namespace syst
} // namespace cfl
