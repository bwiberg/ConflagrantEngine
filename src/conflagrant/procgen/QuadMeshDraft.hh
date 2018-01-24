#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include "MeshDraft.hh"
#include "TriMeshDraft.hh"

namespace cfl {
namespace procgen {
class QuadMeshDraft : public MeshDraft<4> {
public:
    QuadMeshDraft(size_t numVerticesGuess = MeshDraft::DefaultNumVerticesGuess,
                  size_t numPolygonsGuess = MeshDraft::DefaultNumPolygonsGuess);

    void AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3,
                 vec3 normal0, vec3 normal1, vec3 normal2, vec3 normal3) override;

    using MeshDraft<4>::AddQuad;

    TriMeshDraft ClearAndExportTriMeshDraft();
};

inline QuadMeshDraft::QuadMeshDraft(size_t numVerticesGuess, size_t numPolygonsGuess)
        : MeshDraft(numVerticesGuess, numPolygonsGuess) {}

inline void QuadMeshDraft::AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3,
                                   vec3 normal0, vec3 normal1, vec3 normal2, vec3 normal3) {
    auto const offset = static_cast<uint>(positions.size());
    polygons.push_back({0 + offset, 1 + offset, 2 + offset, 3 + offset});

    positions.emplace_back(vertex0);
    positions.emplace_back(vertex1);
    positions.emplace_back(vertex2);
    positions.emplace_back(vertex3);

    normals.emplace_back(normal0);
    normals.emplace_back(normal1);
    normals.emplace_back(normal2);
    normals.emplace_back(normal3);
}

inline TriMeshDraft QuadMeshDraft::ClearAndExportTriMeshDraft() {
    TriMeshDraft tri(0, polygons.size() * 2);

    tri.positions = std::move(positions);
    tri.normals = std::move(normals);
    tri.tangents = std::move(tangents);
    tri.uvs = std::move(uvs);
    tri.colors = std::move(colors);

    for (size_t i = 0; i < polygons.size(); ++i) {
        // convert quad to two triangles
        auto const &quad = polygons[i];
        tri.polygons.push_back({quad[0], quad[1], quad[2]});
        tri.polygons.push_back({quad[0], quad[2], quad[3]});
    }

    polygons.clear();

    return tri;
}
} // namespace procgen
} // namespace cfl
