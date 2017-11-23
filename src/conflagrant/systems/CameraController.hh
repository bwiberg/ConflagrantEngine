#pragma once

#include <conflagrant/System.hh>
#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/components/PerspectiveCamera.hh>
#include <conflagrant/components/OrthographicCamera.hh>
#include <conflagrant/components/Transform.hh>

#include <conflagrant/events.hh>
#include <entityx/System.h>

namespace cfl {
namespace syst {
struct CameraController : public cfl::System, public entityx::System<CameraController> {
    entityx::Entity activeCamera;

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
        if (!activeCamera.valid()) {
            std::vector<entityx::Entity> cameraEntities;

            // gather all perspective- and orthographic cameras
            for (auto entity : entities.entities_with_components<comp::Transform, comp::PerspectiveCamera>())
                cameraEntities.push_back(entity);
            for (auto entity : entities.entities_with_components<comp::Transform, comp::OrthographicCamera>())
                cameraEntities.push_back(entity);

            if (cameraEntities.empty()) {
                // create default camera
                auto entity = entities.create();
                entity.assign<comp::Name>()->value = "(auto-created camera)";
                entity.assign<comp::Transform>();
                entity.assign<comp::PerspectiveCamera>();
                cameraEntities.push_back(entity);
            }

            // select the first camera
            activeCamera = cameraEntities[0];

            // broadcast that the active camera changed
            events.emit(event::CameraChanged{.oldCamera = entityx::Entity(), .newCamera = activeCamera});
        }

        // update camera
        auto transform = activeCamera.component<comp::Transform>();
        auto perspective = activeCamera.component<comp::PerspectiveCamera>();
        auto orthographic = activeCamera.component<comp::OrthographicCamera>();

        uint const width = 640, height = 480;

        if (perspective) {
            if (perspective->hasChanged) {
                perspective->projection = glm::perspectiveFov(perspective->fov,
                                                              static_cast<decltype(perspective->fov)>(width),
                                                              static_cast<decltype(perspective->fov)>(height),
                                                              perspective->zNear, perspective->zFar);
                perspective->hasChanged = false;
            }
        } else if (orthographic) {
            if (orthographic->hasChanged) {
                float right = orthographic->size * width / 2;
                float left = -right;
                float bottom = orthographic->size * height / 2;
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
        string cameraName = "none";
        if (sys.activeCamera.valid()) {
            cameraName = sys.activeCamera.component<comp::Name>()->value;
        }
        ImGui::LabelText("Active camera", cameraName.c_str());
        return true;
    }
};
} // namespace syst
} // namespace cfl
