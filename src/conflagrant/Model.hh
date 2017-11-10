#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/assets/Asset.hh>
#include <conflagrant/Mesh.hh>
#include <conflagrant/Material.hh>

#include <vector>

namespace cfl {
using assets::Asset;

/**
 * @brief CPU representation of a triangle mesh.
 */
struct Model : public Asset {
    /**
     * @brief The vertices of the mesh.
     */
    std::vector<std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>>> meshes;

    /**
     * @brief The indices of each triangle.
     */
    std::vector<uvec3> triangles;
};
} // namespace cfl
