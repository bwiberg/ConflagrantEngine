#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

namespace cfl {
namespace comp {
struct OrthographicCamera {
    float size{1.0f}, zNear{0.01f}, zFar{100.0f};

    bool hasChanged{true};

    mat4 projection;

    inline static string const GetName() {
        return "OrthographicCamera";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, OrthographicCamera &camera) {
        SERIALIZE(json["size"], camera.size);
        SERIALIZE(json["near"], camera.zNear);
        SERIALIZE(json["far"], camera.zFar);
        camera.hasChanged = true;
        return true;
    }

    inline static bool DrawWithImGui(OrthographicCamera &camera, InputManager const &input) {
        float const DragSpeed = (input.GetKey(Key::LEFT_CONTROL) || input.GetKey(Key::LEFT_SHIFT))
                                ? 0.01f : 0.5f;

        camera.hasChanged |= ImGui::DragFloat("Size", &camera.size, DragSpeed);
        camera.hasChanged |= ImGui::DragFloat("Near clip", &camera.zNear, DragSpeed);
        camera.hasChanged |= ImGui::DragFloat("Far clip", &camera.zFar, DragSpeed);

        return true;
    }
};
} // namespace comp
} // namespace cfl
