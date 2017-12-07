#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/InputManager.hh>
#include <conflagrant/geometry.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct BoundingSphere {
    static constexpr auto ComponentName = "BoundingSphere";

    geometry::Sphere sphere;

    inline static bool DrawWithImGui(BoundingSphere &comp, InputManager const &input) {
        ImGui::Text("Center: %4.3f, %4.3f, %4.3f", comp.sphere.center.x, comp.sphere.center.y, comp.sphere.center.z);
        ImGui::Text("Radius: %4.3f", comp.sphere.radius);
        return true;
    }
};
} // namespace comp
} // namespace cfl
