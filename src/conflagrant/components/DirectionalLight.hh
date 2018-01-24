#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/serialization/glm.hh>
#include <conflagrant/InputManager.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct DirectionalLight {
    static constexpr auto ComponentName = "DirectionalLight";

    float horizontal, vertical;
    vec3 color{1.0f, 1.0f, 1.0f};
    float intensity{1.0f};
    bool castShadows{false};

    inline static bool Serialize(BaseSerializer const& serializer, Json::Value &json,
                                 DirectionalLight &comp) {
        SERIALIZE(cfl::comp::DirectionalLight, json["color"], comp.color);
        SERIALIZE(cfl::comp::DirectionalLight, json["intensity"], comp.intensity);
        SERIALIZE(cfl::comp::DirectionalLight, json["horizontal"], comp.horizontal);
        SERIALIZE(cfl::comp::DirectionalLight, json["vertical"], comp.vertical);
        SERIALIZE(cfl::comp::DirectionalLight, json["castShadows"], comp.castShadows);
        return true;
    }

    inline static bool DrawWithImGui(DirectionalLight &comp, InputManager const &input) {
        float const DragSpeed = (input.GetKey(Key::LEFT_CONTROL) || input.GetKey(Key::LEFT_SHIFT))
                                ? 0.01f : 0.5f;

        ImGui::DragFloat("Horizontal angle", &comp.horizontal, DragSpeed);
        ImGui::DragFloat("Vertical angle", &comp.vertical, DragSpeed, -90, 90);
        ImGui::DragFloat3("Color", glm::value_ptr(comp.color), DragSpeed);
        ImGui::DragFloat("Intensity", &comp.intensity, DragSpeed);
        ImGui::Checkbox("Casts shadows", &comp.castShadows);

        return true;
    }
};
} // namespace comp
} // namespace cfl
