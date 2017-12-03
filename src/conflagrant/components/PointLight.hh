#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/serialization/glm.hh>
#include <conflagrant/InputManager.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct PointLight {
    vec3 color{1.0f, 1.0f, 1.0f};
    float intensity{1.0f};

    inline static string const &GetName() {
        $
        static const string name = "PointLight";
        return name;
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, PointLight &comp) {
        $
        SERIALIZE_CUSTOM(json["color"], comp.color);
        SERIALIZE(json["intensity"], comp.intensity);
        return true;
    }

    inline static bool DrawWithImGui(PointLight &comp, InputManager const &input) {
        $
        float const DragSpeed = (input.GetKey(Key::LEFT_CONTROL) || input.GetKey(Key::LEFT_SHIFT))
                                ? 0.01f : 0.5f;

        ImGui::DragFloat3("Color", glm::value_ptr(comp.color), DragSpeed);
        ImGui::DragFloat("Intensity", &comp.intensity, DragSpeed);

        return true;
    }
};
} // namespace comp
} // namespace cfl
