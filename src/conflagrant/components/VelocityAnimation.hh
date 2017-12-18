#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/serialization/glm.hh>
#include <conflagrant/InputManager.hh>
#include <conflagrant/Time.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct VelocityAnimation {
    static constexpr auto ComponentName = "VelocityAnimation";

    bool autoStart{false};
    vec3 linearVelocity{0.f}, angularVelocity{0.f};

    // state
    bool isRunning{false};

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 VelocityAnimation &comp) {
        SERIALIZE(cfl::comp::Animation, json["autoStart"], comp.autoStart);
        SERIALIZE(cfl::comp::Animation, json["linearVelocity"], comp.linearVelocity);
        SERIALIZE(cfl::comp::Animation, json["angularVelocity"], comp.angularVelocity);

        if (serializer.IsDeserializer()) {
            comp.isRunning = comp.autoStart;
        }

        return true;
    }

    inline static bool DrawWithImGui(VelocityAnimation &comp, InputManager const &input) {
        ImGui::Checkbox("Autostart", &comp.autoStart);
        ImGui::DragFloat3("Linear velocity", glm::value_ptr(comp.linearVelocity));
        ImGui::DragFloat3("Angular velocity", glm::value_ptr(comp.angularVelocity));

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
