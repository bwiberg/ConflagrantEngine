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

    inline void Reset(comp::Model &model) {
        // update bounding volumes
        vec3 min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min());

        for (auto const& part : model.value->parts) {
            auto const& center = part.first->boundingSphere.center;
            vec3 radius(part.first->boundingSphere.radius);

            min = glm::min(min, center - radius);
            max = glm::max(max, center + radius);
        }

        vec3 const center = 0.5f * (min + max);
        float const radius = glm::max(glm::distance(min, center), glm::distance(max, center));
        sphere.center = center;
        sphere.radius = radius;
    }
};
} // namespace comp
} // namespace cfl
