#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct Name {
    static constexpr auto ComponentName = "Name";

    string value;

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 Name &name) {
        $
        SERIALIZE(cfl::comp::Name, json, name.value);
        return true;
    }

    inline static bool DrawWithImGui(Name &name, InputManager const &input) {
        $
        size_t constexpr BufferSize = 1024;
        char buf[BufferSize] = {'\0'};
        name.value.copy(buf, BufferSize);
        if (ImGui::InputText("Text", buf, IM_ARRAYSIZE(buf))) {
            name.value = string(buf);
        }

        return true;
    }
};
} // namespace comp
} // namespace cfl
