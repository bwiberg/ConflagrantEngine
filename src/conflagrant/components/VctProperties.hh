#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/InputManager.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct VctProperties {
    static constexpr auto ComponentName = "VctProperties";

    float specularReflectance{0.f}, radiance{1.f};

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 VctProperties &comp) {
        SERIALIZE(cfl::comp::VctProperties, json["specularReflectance"], comp.specularReflectance);
        SERIALIZE(cfl::comp::VctProperties, json["radiance"], comp.radiance);
        return true;
    }

    inline static bool DrawWithImGui(VctProperties &comp, InputManager const &input) {
        ImGui::DragFloat("Specular reflectance", &comp.specularReflectance, 1.0f, 0.0f, 20.0f);
        ImGui::DragFloat("Radiance", &comp.radiance, 1.0f, 0.0f, 30.0f);

        return true;
    }
};
} // namespace comp
} // namespace cfl
