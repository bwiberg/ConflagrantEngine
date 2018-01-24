/**
 * Created by bwiberg on 9/13/17.
 */

#pragma once

#include <conflagrant/GL.hh>
#include <conflagrant/gl/Mesh.hh>
#include <conflagrant/assets/Mesh.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
/**
 * @brief Component that references a Mesh asset
 */
struct Mesh {
    static constexpr auto ComponentName = "Mesh";

    std::shared_ptr<assets::Mesh> value;

    inline static bool DrawWithImGui(Mesh &mesh, InputManager const &input) {
        if (mesh.value) {
            ImGui::LabelText("Vertices", "%i", mesh.value->vertices.size());
            ImGui::LabelText("Triangles", "%i", mesh.value->triangles.size());
        } else {
            ImGui::TextColored(ImVec4(1.f, .5f, .5f, 1.f), "nullptr");
        }
        return true;
    }
};
} // namespace comp
} // namespace cfl
