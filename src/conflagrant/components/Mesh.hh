/**
 * Created by bwiberg on 9/13/17.
 */

#pragma once

#include <conflagrant/GL.hh>
#include <conflagrant/gl/Mesh.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
/**
 * @brief Component that references a Mesh asset
 */
struct Mesh {
    std::shared_ptr<assets::Mesh> value;

    inline static string const GetName() {
        return "Mesh";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, Mesh &mesh) {
        // SERIALIZE(json, name.value);
        return true;
    }

    inline static bool DrawWithImGui(Mesh &mesh, InputManager const &input) {
        return true;
    }
};
} // namespace comp
} // namespace cfl
