#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/InputManager.hh>
#include <conflagrant/geometry.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct BoundingSphere {
    geometry::Sphere sphere;

    inline static string const &GetName() {
        static const string name = "BoundingSphere";
        return name;
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, BoundingSphere &comp) {
        // SERIALIZE(json, ...);
        return true;
    }

    inline static bool DrawWithImGui(BoundingSphere &comp, InputManager const &input) {
        ImGui::Text("Center: %4.3f, %4.3f, %4.3f", comp.sphere.center.x, comp.sphere.center.y, comp.sphere.center.z);
        ImGui::Text("Radius: %4.3f", comp.sphere.radius);
        return true;
    }
};
} // namespace comp
} // namespace cfl
