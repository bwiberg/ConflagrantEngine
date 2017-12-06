#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/serialization/glm.hh>
#include <conflagrant/InputManager.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct DirectionalLight {
    float horizontal, vertical;
    vec3 color{1.0f, 1.0f, 1.0f};
    float intensity{1.0f};
    bool castShadows{false};

    inline static string const &GetName() {
        $
        static const string name = "DirectionalLight";
        return name;
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, DirectionalLight &comp) {
        $
        SERIALIZE_CUSTOM(json["color"], comp.color);
        SERIALIZE(json["intensity"], comp.intensity);
        SERIALIZE(json["horizontal"], comp.horizontal);
        SERIALIZE(json["vertical"], comp.vertical);
        SERIALIZE(json["castShadows"], comp.castShadows);
        return true;
    }

    inline static bool DrawWithImGui(DirectionalLight &comp, InputManager const &input) {
        $
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
