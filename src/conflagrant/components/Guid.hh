#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <crossguid/Guid.hpp>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/serialization/crossguid.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct Guid {
    static constexpr auto ComponentName = "Guid";
    static constexpr auto RuntimeOnly = false;

    inline Guid() : value(xg::newGuid()) {}

    xg::Guid value;

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 Guid &guid) {
        $
        SERIALIZE(cfl::comp::Guid, json, guid.value);
        return true;
    }

    inline static bool DrawWithImGui(Guid &guid, InputManager const &input) {
        $
        ImGui::LabelText("Guid", guid.value.str().c_str());

        if (ImGui::Button("Regenerate Guid")) {
            guid.value = xg::newGuid();
        }

        return true;
    }
};
} // namespace comp
} // namespace cfl
