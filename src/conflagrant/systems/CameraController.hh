#pragma once

#include <conflagrant/System.hh>
#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
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
    float movementSpeed{3};
    float turnSpeed{0.3};

    bool isActive;

    void MoveCamera(comp::Transform &transform);

public:
    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;

    inline static string GetName() {
        $
        return "CameraController";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, CameraController &sys) {
        $
        json["name"] = GetName();
        return true;
    }

    static bool DrawWithImGui(CameraController &sys, InputManager const &input);
};
} // namespace syst
} // namespace cfl
