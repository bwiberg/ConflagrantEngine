#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/assets/Asset.hh>
#include <conflagrant/gl/Mesh.hh>

#include <vector>

namespace cfl {
namespace assets {
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

    bool UploadToGL();

    /**
     * @brief The vertices of the mesh.
     */
    std::vector<Vertex> vertices;

    /**
     * @brief The indices of each triangle.
     */
    std::vector<uvec3> triangles;

    /**
     * @brief Set this flag if the glMesh field needs to be updated.
     */
    bool glMeshNeedsUpdate{true};

    /**
     * @brief GPU representation of mesh
     */
    std::shared_ptr<gl::Mesh> glMesh{nullptr};
};

inline bool Mesh::UploadToGL() {
    if (!glMeshNeedsUpdate) return false;

    if (!glMesh) {
        glMesh = std::make_shared<gl::Mesh>();

        glMesh->Attribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, position));
        glMesh->Attribute(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, normal));
        glMesh->Attribute(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, tangent));
        glMesh->Attribute(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, bitangent));
        glMesh->Attribute(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) offsetof(Vertex, texCoord));
    }

    glMesh->BufferVertexData(sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
    glMesh->BufferIndexData(GL_UNSIGNED_INT, 3 * triangles.size(), triangles.data(), GL_DYNAMIC_DRAW, GL_TRIANGLES);

    return true;
}
} // namespace assets
} // namespace cfl
