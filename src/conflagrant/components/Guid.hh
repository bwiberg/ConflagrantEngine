#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <crossguid/Guid.hpp>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/serialization/crossguid.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct Guid {
    xg::Guid value;

    inline static string const GetName() {
        return "Guid";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, Guid &guid) {
        return cfl::Serialize<TSerializer>(json, guid.value);
    }

    static bool DrawWithImGui(Guid &guid) {
        ImGui::LabelText("Guid", guid.value.str().c_str());

        if (ImGui::Button("Regenerate Guid")) {
            guid.value = xg::newGuid();
        }

        return true;
    }
};
} // namespace comp
} // namespace cfl
