#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/serialization/glm.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct Transform {
    vec3 pivot;
    vec3 position;
    quat orientation;
    float scale;

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

    static bool DrawWithImGui(Transform &transform) {
        transform.hasChanged |= ImGui::InputFloat3("Pivot", glm::value_ptr(transform.pivot));
        transform.hasChanged |= ImGui::InputFloat3("Position", glm::value_ptr(transform.position));
        transform.hasChanged |= ImGui::InputFloat4("Orientation", glm::value_ptr(transform.orientation));
        transform.hasChanged |= ImGui::InputFloat("Scale", &transform.scale);

        return true;
    }
};
} // namespace comp
} // namespace cfl
