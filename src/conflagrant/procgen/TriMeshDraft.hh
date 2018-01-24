#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include "MeshDraft.hh"

namespace cfl {
namespace procgen {
class TriMeshDraft : public MeshDraft<3> {
public:
    TriMeshDraft(size_t numVerticesGuess = MeshDraft::DefaultNumVerticesGuess,
                 size_t numPolygonsGuess = MeshDraft::DefaultNumPolygonsGuess);

    void AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3, vec3 normal0, vec3 normal1, vec3 normal2,
                 vec3 normal3) override;

    using MeshDraft<3>::AddQuad;

    void AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2);

    void AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 normal);

    void AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 normal0, vec3 normal1,
                     vec3 normal2);

    void AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec2 uv0, vec2 uv1,
                     vec2 uv2);

    void AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 normal, vec2 uv0,
                     vec2 uv1, vec2 uv2);

    void AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 normal0, vec3 normal1,
                     vec3 normal2, vec2 uv0, vec2 uv1, vec2 uv2);

};

inline TriMeshDraft::TriMeshDraft(size_t numVerticesGuess, size_t numPolygonsGuess)
        : MeshDraft(numVerticesGuess, numPolygonsGuess) {}

inline void TriMeshDraft::AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3,
                                  vec3 normal0, vec3 normal1, vec3 normal2, vec3 normal3) {
    auto const offset = static_cast<uint>(positions.size());
    polygons.push_back({0 + offset, 1 + offset, 2 + offset});
    polygons.push_back({0 + offset, 2 + offset, 3 + offset});

    positions.emplace_back(vertex0);
    positions.emplace_back(vertex1);
    positions.emplace_back(vertex2);
    positions.emplace_back(vertex3);

    normals.emplace_back(normal0);
    normals.emplace_back(normal1);
    normals.emplace_back(normal2);
    normals.emplace_back(normal3);
}

inline void TriMeshDraft::AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2) {
    vec3 normal = glm::normalize(glm::cross(vertex1 - vertex0, vertex2 - vertex1));
    AddTriangle(vertex0, vertex1, vertex2, normal);
}

inline void TriMeshDraft::AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 normal) {
    AddTriangle(vertex0, vertex1, vertex2, normal, normal, normal);
}

inline void TriMeshDraft::AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2,
                                      vec3 normal0, vec3 normal1, vec3 normal2) {
    auto const offset = static_cast<uint>(positions.size());
    polygons.push_back({0 + offset, 1 + offset, 2 + offset});

    positions.emplace_back(vertex0);
    positions.emplace_back(vertex1);
    positions.emplace_back(vertex2);

    normals.emplace_back(normal0);
    normals.emplace_back(normal1);
    normals.emplace_back(normal2);
}

inline void TriMeshDraft::AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2,
                                      vec2 uv0, vec2 uv1, vec2 uv2) {
    uvs.emplace_back(uv0);
    uvs.emplace_back(uv1);
    uvs.emplace_back(uv2);
    AddTriangle(vertex0, vertex1, vertex2);
}

inline void TriMeshDraft::AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2,
                                      vec3 normal,
                                      vec2 uv0, vec2 uv1, vec2 uv2) {
    uvs.emplace_back(uv0);
    uvs.emplace_back(uv1);
    uvs.emplace_back(uv2);
    AddTriangle(vertex0, vertex1, vertex2, normal);
}

inline void TriMeshDraft::AddTriangle(vec3 vertex0, vec3 vertex1, vec3 vertex2,
                                      vec3 normal0, vec3 normal1, vec3 normal2,
                                      vec2 uv0, vec2 uv1, vec2 uv2) {
    uvs.emplace_back(uv0);
    uvs.emplace_back(uv1);
    uvs.emplace_back(uv2);
    AddTriangle(vertex0, vertex1, vertex2, normal0, normal1, normal2);
}
} // namespace procgen
} // namespace cfl
