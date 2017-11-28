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
    float movementSpeed{1};
    float turnSpeed{1};

    inline entityx::Entity GetActiveCamera(entityx::EntityManager &entities) {
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
            activeCamera.assign<comp::Transform>()->Position(vec3(0, 0, 10));
            activeCamera.assign<comp::PerspectiveCamera>();
            // activeCamera.assign<comp::OrthographicCamera>();
            activeCamera.assign<comp::ActiveCamera>();
        }

        return activeCamera;
    }

    void MoveCamera(comp::Transform &transform) {
        mat4 const& matrix = transform.GetMatrix();
        vec3 position = transform.Position();
        vec3 eulerAnglesDegrees = transform.EulerAnglesDegrees();

        vec3 const Forward = matrix * vec4(0.0f, 0.0f, 1.0f, 1.0f);
        vec3 const Right = matrix * vec4(1.0f, 0.0f, 0.0f, 1.0f);
        vec3 const Up = matrix * vec4(0.0f, 1.0f, 0.0f, 1.0f);

        float const inputUp = (input->GetKey(Key::W) || input->GetKey(Key::UP)) ? 1.0f : 0.0f;
        float const inputDown = (input->GetKey(Key::S) || input->GetKey(Key::DOWN)) ? 1.0f : 0.0f;
        float const inputRight = (input->GetKey(Key::D) || input->GetKey(Key::RIGHT)) ? 1.0f : 0.0f;
        float const inputLeft = (input->GetKey(Key::A) || input->GetKey(Key::LEFT)) ? 1.0f : 0.0f;

        float const amountForward = inputUp - inputDown;
        float const amountRight = inputRight - inputLeft;

        vec3 const moveForward = amountForward * vec3(matrix * vec4(0.0f, 0.0f, 1.0f, 1.0f));
        vec3 const moveSideways = amountRight * vec3(matrix * vec4(1.0f, 0.0f, 0.0f, 1.0f));

        position += Time::DeltaTime<float>() * movementSpeed * (moveForward + moveSideways);

        if ((input->GetKey(Key::LEFT_SHIFT) && input->GetKeyDown(Key::F)) ||
            (input->GetKeyDown(Key::LEFT_SHIFT) && input->GetKey(Key::F))) {
            auto cursorMode = window->GetCursorMode();
            if (cursorMode == CursorMode::NORMAL) {
                window->SetCursorMode(CursorMode::HIDDEN_FIXED);
            } else {
                window->SetCursorMode(CursorMode::NORMAL);
            }
        }

        dvec2 const delta = input->GetDeltaMousePositionPixel();

        eulerAnglesDegrees.x += Time::DeltaTime<float>() * static_cast<float>(turnSpeed * delta.y);
        eulerAnglesDegrees.y += Time::DeltaTime<float>() * static_cast<float>(turnSpeed * delta.x);

        transform.Position(position);
        transform.EulerAnglesDegrees(eulerAnglesDegrees);
    }

public:
    inline void
    update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
        using entityx::ComponentHandle;

        auto activeCamera = GetActiveCamera(entities);

        // update camera
        auto transform = activeCamera.component<comp::Transform>();
        auto perspective = activeCamera.component<comp::PerspectiveCamera>();
        auto orthographic = activeCamera.component<comp::OrthographicCamera>();

        MoveCamera(*transform);

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
        ImGui::DragFloat("Movement speed", &sys.movementSpeed, 1.0f, 0);
        return true;
    }
};
} // namespace syst
} // namespace cfl
