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
struct PeriodicalAnimation {
    static constexpr auto ComponentName = "PeriodicalAnimation";

    enum class Type {
        SMOOTH,
        SHARP
    } type{Type::SMOOTH};

    bool autoStart{false};
    time_t periodSeconds{1};
    vec3 deltaPosition, deltaRotation;

    bool isRunning{false}, hasSavedStartValues{false}, wasRunning;
    cfl::time_t startTime;
    vec3 startPosition;
    quat startRotation;

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 PeriodicalAnimation &comp) {
        if (serializer.IsSerializer()) {
            json["type"] = (comp.type == Type::SMOOTH ? "SMOOTH" : "SHARP");
        } else {
            comp.type = json["type"].asString() == "SMOOTH" ? Type::SMOOTH : Type::SHARP;
        }

        SERIALIZE(cfl::comp::PeriodicalAnimation, json["autoStart"], comp.autoStart);
        SERIALIZE(cfl::comp::PeriodicalAnimation, json["periodSeconds"], comp.periodSeconds);
        SERIALIZE(cfl::comp::PeriodicalAnimation, json["deltaPosition"], comp.deltaPosition);
        SERIALIZE(cfl::comp::PeriodicalAnimation, json["deltaRotation"], comp.deltaRotation);

        if (serializer.IsDeserializer()) {
            comp.isRunning = comp.autoStart;
            comp.startTime = Time::CurrentTime();
            comp.hasSavedStartValues = false;
        }

        return true;
    }

    inline static bool DrawWithImGui(PeriodicalAnimation &comp, InputManager const &input) {
        ImGui::Checkbox("Autostart", &comp.autoStart);

        float p = static_cast<float>(comp.periodSeconds);
        ImGui::DragFloat("Period (seconds)", &p, 1.0f, 0.1f);
        comp.periodSeconds = static_cast<cfl::time_t>(p);

        ImGui::DragFloat3("Delta position", glm::value_ptr(comp.deltaPosition));
        ImGui::DragFloat3("Delta rotation", glm::value_ptr(comp.deltaRotation));

        if (!comp.isRunning && ImGui::Button("Start")) {
            comp.isRunning = true;
            comp.startTime = Time::CurrentTime();
            comp.hasSavedStartValues = false;
        }

        else if (comp.isRunning && ImGui::Button("Stop")) {
            comp.isRunning = false;
            comp.wasRunning = true;
        }

        return true;
    }
};
} // namespace comp
} // namespace cfl
