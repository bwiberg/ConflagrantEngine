#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/serialization/glm.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct Transform {
    vec3 pivot{0.0f, 0.0f, 0.0f};
    vec3 position{0.0f, 0.0f, 0.0f};
    quat orientation{vec3(0.0f, 0.0f, 0.0f)};
    float scale{1.0f};

    bool hasChanged{true};

    mat4 matrix;

    inline static string const GetName() {
        return "Transform";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, Transform &transform) {
        SERIALIZE_CUSTOM(json["pivot"], transform.pivot);
        SERIALIZE_CUSTOM(json["position"], transform.position);
        SERIALIZE_CUSTOM(json["orientation"], transform.orientation);
        SERIALIZE(json["scale"], transform.scale);
        transform.hasChanged = true;
        return true;
    }

    static bool DrawWithImGui(Transform &transform, InputManager const &input) {
        float const DragSpeed = (input.GetKey(Key::LEFT_CONTROL) || input.GetKey(Key::LEFT_SHIFT))
                                ? 0.01f : 0.5f;

        transform.hasChanged |= ImGui::DragFloat3("Pivot", glm::value_ptr(transform.pivot), DragSpeed);
        transform.hasChanged |= ImGui::DragFloat3("Position", glm::value_ptr(transform.position), DragSpeed);

        vec3 eulerAngles = glm::degrees(glm::eulerAngles(transform.orientation));
        transform.hasChanged |= ImGui::DragFloat3("Orientation", glm::value_ptr(eulerAngles), DragSpeed);
        transform.orientation = glm::quat(glm::radians(eulerAngles));

        transform.hasChanged |= ImGui::DragFloat("Scale", &transform.scale, DragSpeed);

        return true;
    }
};
} // namespace comp
} // namespace cfl
