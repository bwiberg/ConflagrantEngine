#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/assets/Asset.hh>

#include <vector>

namespace cfl {
namespace geometry {

using assets::Asset;

/**
 * @brief CPU representation of a triangle mesh
 */
struct Mesh : public Asset {
    /**
     * @brief Stores the position of each vertex.
     */
    std::vector<vec3> positions;

    /**
     * @brief Stores the normal of each vertex.
     */
    std::vector<vec3> normals;

    /**
     * @brief Stores the texture coordinate(s) of each vertex.
     */
    std::vector<vec2> texCoords;

    /**
     * @brief Stores the indices of each triangle.
     */
    std::vector<uvec3> triangles;
};
} // namespace geometry
} // namespace cfl
