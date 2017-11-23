#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

namespace cfl {
namespace comp {
struct PerspectiveCamera {
    float fov{60.0f}, zNear{0.01f}, zFar{100.0f};

    bool hasChanged{true};

    mat4 projection;

    inline static string const GetName() {
        return "PerspectiveCamera";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, PerspectiveCamera &camera) {
        SERIALIZE(json["fov"], camera.fov);
        SERIALIZE(json["zNear"], camera.zNear);
        SERIALIZE(json["zFar"], camera.zFar);
        camera.hasChanged = true;
        return true;
    }

    static bool DrawWithImGui(PerspectiveCamera &camera, InputManager const &input) {
        float const DragSpeed = (input.GetKey(Key::LEFT_CONTROL) || input.GetKey(Key::LEFT_SHIFT))
                                ? 0.01f : 0.5f;

        camera.hasChanged |= ImGui::DragFloat("Field of view", &camera.fov, DragSpeed);
        camera.hasChanged |= ImGui::DragFloat("Near clip", &camera.zNear, DragSpeed);
        camera.hasChanged |= ImGui::DragFloat("Far clip", &camera.zFar, DragSpeed);

        return true;
    }
};
} // namespace comp
} // namespace cfl
