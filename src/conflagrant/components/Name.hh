#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct Name {
    string value;

    inline static string const GetName() {
        $
        return "Name";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, Name &name) {
        $
        SERIALIZE(json, name.value);
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
