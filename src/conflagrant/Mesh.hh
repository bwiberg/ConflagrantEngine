/**
 * Created by bwiberg on 9/13/17.
 */

#pragma once

#include <vector>
#include "types.hh"

namespace cfl {
/**
 * @brief Basic mesh data structure that simply holds data.
 */
struct Mesh {
    /**
     * @brief Constructs a Mesh instance.
     * @param positions The position of each vertex.
     * @param normals The normal of each vertex.
     * @param texCoords The texture coordinate(s) of each vertex.
     * @param colors The color of each vertex.
     * @param triangles The indices of each triangle.
     */
    Mesh(std::vector<vec3> const &positions,
         std::vector<vec3> const &normals,
         std::vector<vec2> const &texCoords,
         std::vector<vec3> const &colors,
         std::vector<uvec3> const &triangles);

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
     * @brief Stores the color of each vertex.
     */
    std::vector<vec3> colors;

    /**
     * @brief Stores the indices of each triangle.
     */
    std::vector<uvec3> triangles;
};
}

inline cfl::Mesh::Mesh(std::vector<vec3> const &positions,
                       std::vector<vec3> const &normals,
                       std::vector<vec2> const &texCoords,
                       std::vector<vec3> const &colors,
                       std::vector<uvec3> const &triangles)
        : positions(positions),
          normals(normals),
          texCoords(texCoords),
          colors(colors),
          triangles(triangles) {}
