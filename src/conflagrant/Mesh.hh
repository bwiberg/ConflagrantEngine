#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/assets/Asset.hh>

#include <vector>

namespace cfl {
using assets::Asset;

/**
 * @brief CPU representation of a vertex.
 */
struct Vertex {
    vec3 position{0};
    vec3 normal{0};
    vec3 tangent{0};
    vec3 bitangent{0};
    vec2 texCoord{0};
};

/**
 * @brief CPU representation of a triangle mesh.
 */
struct Mesh : public Asset {
    inline Mesh(std::vector<Vertex> &&vertices, std::vector<uvec3> &&triangles)
            : vertices(vertices),
              triangles(triangles) {}

    /**
     * @brief The vertices of the mesh.
     */
    std::vector<Vertex> vertices;

    /**
     * @brief The indices of each triangle.
     */
    std::vector<uvec3> triangles;
};
} // namespace cfl
