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
    static constexpr auto ComponentName = "Mesh";

    std::shared_ptr<assets::Mesh> value;

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 Mesh &mesh) {
        $
        return true;
    }

    inline static bool DrawWithImGui(Mesh &mesh, InputManager const &input) {
        $
        return true;
    }
};
} // namespace comp
} // namespace cfl
