#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/InputManager.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct DirectionalLightAnimation {
    static constexpr auto ComponentName = "DirectionalLightAnimation";

    float horizontalSpeed{0.f}, verticalSpeed{0.f};
    bool autoStart{false};

    bool isRunning{false};

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 DirectionalLightAnimation &comp) {
        SERIALIZE(cfl::comp::DirectionalLightAnimation, json["autoStart"], comp.autoStart);
        SERIALIZE(cfl::comp::DirectionalLightAnimation, json["horizontalSpeed"], comp.horizontalSpeed);
        SERIALIZE(cfl::comp::DirectionalLightAnimation, json["verticalSpeed"], comp.verticalSpeed);

        if (serializer.IsDeserializer()) {
            comp.isRunning = comp.autoStart;
        }

        return true;
    }

    inline static bool DrawWithImGui(DirectionalLightAnimation &comp, InputManager const &input) {
        ImGui::Checkbox("Autostart", &comp.autoStart);
        ImGui::DragFloat("Horizontal speed", &comp.horizontalSpeed);
        ImGui::DragFloat("Vertical speed", &comp.verticalSpeed);

        if (!comp.isRunning && ImGui::Button("Start")) {
            comp.isRunning = true;
        }

        else if (comp.isRunning && ImGui::Button("Stop")) {
            comp.isRunning = false;
        }

        return true;
    }
};
} // namespace comp
} // namespace cfl
