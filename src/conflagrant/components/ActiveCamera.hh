#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/InputManager.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct ActiveCamera {
    inline static string const &GetName() {
        static const string name = "ActiveCamera";
        return name;
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, ActiveCamera &comp) {
        // SERIALIZE(json, ...);
        return true;
    }

    inline static bool DrawWithImGui(ActiveCamera &comp, InputManager const &input) {
        // ImGui:: ...
        return true;
    }
};
} // namespace comp
} // namespace cfl
