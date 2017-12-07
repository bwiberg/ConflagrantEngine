#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>

#include <conflagrant/assets/Asset.hh>
#include <conflagrant/assets/Mesh.hh>
#include <conflagrant/assets/Material.hh>

#include <vector>

namespace cfl {
namespace assets {
/**
 * @brief CPU representation of a triangle mesh.
 */
struct Model : public Asset {
    /**
     * @brief The vertices of the mesh.
     */
    std::vector<std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>>> parts;
};
} // namespace assets
} // namespace cfl
