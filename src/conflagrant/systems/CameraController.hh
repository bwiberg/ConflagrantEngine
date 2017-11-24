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

namespace cfl {
namespace syst {
struct CameraController : public cfl::System, public entityx::System<CameraController> {
    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
        using entityx::ComponentHandle;

        ComponentHandle<comp::Transform> transform;
        ComponentHandle<comp::ActiveCamera> active;
        ComponentHandle<comp::PerspectiveCamera> perspective;
        ComponentHandle<comp::OrthographicCamera> orthographic;

        entityx::Entity activeCamera;
        for (auto entity : entities.entities_with_components(transform, active, perspective)) {
            activeCamera = entity;
            break;
        }

        if (!activeCamera.valid()) {
            for (auto entity : entities.entities_with_components(transform, active, orthographic)) {
                activeCamera = entity;
                break;
            }
        }

        if (!activeCamera.valid()) {
            activeCamera = entities.create();
            activeCamera.assign<comp::Name>()->value = "(auto-created camera)";
            activeCamera.assign<comp::Guid>();
            activeCamera.assign<comp::Transform>()->position.z = 10;
            activeCamera.assign<comp::PerspectiveCamera>();
            // activeCamera.assign<comp::OrthographicCamera>();
            activeCamera.assign<comp::ActiveCamera>();
        }

        // update camera
        transform = activeCamera.component<comp::Transform>();
        perspective = activeCamera.component<comp::PerspectiveCamera>();
        orthographic = activeCamera.component<comp::OrthographicCamera>();

        uvec2 size;
        if (perspective) {
            if (window->SizeHasChanged(size) || perspective->hasChanged) {
                perspective->projection = glm::perspectiveFov(glm::radians(perspective->fov),
                                                              static_cast<float>(size.x),
                                                              static_cast<float>(size.y),
                                                              perspective->zNear, perspective->zFar);
                perspective->hasChanged = false;
            }
        } else if (orthographic) {
            if (window->SizeHasChanged(size) || orthographic->hasChanged) {
                float right = orthographic->size * size.x / 2;
                float left = -right;
                float bottom = orthographic->size * size.y / 2;
                float top = -bottom;

                orthographic->projection = glm::ortho(left, right, bottom, top, orthographic->zNear,
                                                      orthographic->zFar);
                orthographic->hasChanged = false;
            }
        } else {
            LOG_ERROR(cfl::syst::CameraController)
                    << "Active camera entity does not have Perspective-/OrthographicCamera" << std::endl;
        }
    }

    inline static string GetName() {
        return "CameraController";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, CameraController &sys) {
        json["name"] = GetName();
        return true;
    }

    static bool DrawWithImGui(CameraController &sys, InputManager const &input) {
        return true;
    }
};
} // namespace syst
} // namespace cfl
