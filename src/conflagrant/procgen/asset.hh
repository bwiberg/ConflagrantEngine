#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

#include <conflagrant/procgen/TriMeshDraft.hh>
#include <conflagrant/procgen/QuadMeshDraft.hh>
#include <conflagrant/assets/Mesh.hh>

namespace cfl {
namespace procgen {
template<size_t PolygonOrder, typename MeshDraftContainer>
std::shared_ptr<assets::Mesh>
ExportMeshAsset(MeshDraftContainer const &drafts) {
    auto constexpr NumEmittedTrianglesPerPolygon = PolygonOrder - 2;

    size_t numVertices = 0;
    size_t numTriangles = 0;

    for (MeshDraft<PolygonOrder> const &draft : drafts) {
        if (!draft.ValidateVertices() || !draft.ValidatePolygonIndices()) {
            return nullptr;
        }

        numVertices += draft.NumVertices();
        numTriangles += NumEmittedTrianglesPerPolygon * draft.NumPolygons();
    }

    std::vector<assets::Vertex> vertices;
    vertices.reserve(numVertices);

    std::vector<uvec3> triangles;
    triangles.reserve(numTriangles);

    size_t vertexIndexOffset = 0;
    for (auto const &draft : drafts) {
        for (auto i = 0; i < draft.NumVertices(); ++i) {
            auto const bitangent = glm::normalize(glm::cross(draft.normals[i], draft.tangents[i]));
            vertices.emplace_back(draft.positions[i],
                                  draft.normals[i],
                                  draft.tangents[i],
                                  bitangent,
                                  draft.uvs[i]);
        }

        for (auto const &polygon : draft.polygons) {
            for (size_t tri = 0; tri < NumEmittedTrianglesPerPolygon; ++tri) {
                triangles.emplace_back(vertexIndexOffset + polygon[0],
                                       vertexIndexOffset + polygon[tri + 1],
                                       vertexIndexOffset + polygon[tri + 2]);
            }
        }

        vertexIndexOffset += draft.NumVertices();
    }

    vertices.shrink_to_fit();
    triangles.shrink_to_fit();

    return std::make_shared<assets::Mesh>(std::move(vertices), std::move(triangles));
}

template<size_t PolygonOrder>
std::shared_ptr<assets::Mesh> ExportMeshAsset(procgen::MeshDraft<PolygonOrder> const &draft) {
    auto constexpr NumEmittedTrianglesPerPolygon = PolygonOrder - 2;

    if (!draft.ValidateVertices() || !draft.ValidatePolygonIndices()) {
        return nullptr;
    }

    size_t numVertices = draft.NumVertices();
    size_t numTriangles = NumEmittedTrianglesPerPolygon * draft.NumPolygons();

    std::vector<assets::Vertex> vertices;
    vertices.reserve(numVertices);

    std::vector<uvec3> triangles;
    triangles.reserve(numTriangles);

    size_t vertexIndexOffset = 0;
    for (auto i = 0; i < draft.NumVertices(); ++i) {
        auto const bitangent = glm::normalize(glm::cross(draft.normals[i], draft.tangents[i]));
        vertices.emplace_back(draft.positions[i],
                              draft.normals[i],
                              draft.tangents[i],
                              bitangent,
                              draft.uvs[i]);
    }

    for (auto const &polygon : draft.polygons) {
        for (size_t tri = 0; tri < NumEmittedTrianglesPerPolygon; ++tri) {
            triangles.emplace_back(vertexIndexOffset + polygon[0],
                                   vertexIndexOffset + polygon[tri + 1],
                                   vertexIndexOffset + polygon[tri + 2]);
        }
    }

    vertices.shrink_to_fit();
    triangles.shrink_to_fit();

    return std::make_shared<assets::Mesh>(std::move(vertices), std::move(triangles));
}
} // namespace procgen
} // namespace cfl
