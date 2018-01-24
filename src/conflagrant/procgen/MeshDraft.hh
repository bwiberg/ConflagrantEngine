#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/logging.hh>
#include <conflagrant/procgen/VertexIterator.hh>
#include <conflagrant/procgen/PolygonIterator.hh>

namespace cfl {
namespace procgen {
using std::vector;
template<size_t PolygonOrder>
class MeshDraft {
public:
    static size_t constexpr DefaultNumVerticesGuess = 1024;
    static size_t constexpr DefaultNumPolygonsGuess = 256;

    MeshDraft(size_t numVerticesGuess = DefaultNumVerticesGuess,
              size_t numPolygonsGuess = DefaultNumPolygonsGuess);

    vector<vec3> positions, normals, tangents;
    vector<vec2> uvs;
    vector<vec4> colors;

    vector<std::array<uint, PolygonOrder>> polygons;

    inline auto NumVertices() const {
        return positions.size();
    }

    inline auto NumPolygons() const {
        return polygons.size();
    }

    bool ValidateVertices() const;

    bool ValidatePolygonIndices() const;

    void GenerateTangents();

    virtual void AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3,
                         vec3 normal0, vec3 normal1, vec3 normal2, vec3 normal3) = 0;

    void AddQuad(vec3 origin, vec3 width, vec3 height);

    void AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3);

    void AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3, vec3 normal);

    void AddQuad(vec3 origin, vec3 width, vec3 height,
                 vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3);

    void AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3,
                 vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3);

    void AddQuad(vec3 vertex0, vec3 vertex1, vec3 fvertex2, vec3 vertex3, vec3 normal,
                 vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3);

    void AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3,
                 vec3 normal0, vec3 normal1, vec3 normal2, vec3 normal3,
                 vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3);

    inline VertexIterator BeginVertices() {
        return VertexIterator(positions, normals, tangents, uvs, colors);
    }

    inline ConstVertexIterator BeginVertices() const {
        return ConstVertexIterator(positions, normals, tangents, uvs, colors);
    }

    inline VertexIterator EndVertices() {
        return VertexIterator(positions, normals, tangents, uvs, colors, positions.size());
    }

    inline ConstVertexIterator EndVertices() const {
        return ConstVertexIterator(positions, normals, tangents, uvs, colors, positions.size());
    }

    inline PolygonIterator<PolygonOrder> BeginPolygons() {
        return PolygonIterator<PolygonOrder>(positions, normals, tangents, uvs, colors, polygons);
    }

    inline ConstPolygonIterator<PolygonOrder> BeginPolygons() const {
        return ConstPolygonIterator<PolygonOrder>(positions, normals, tangents, uvs, colors, polygons);
    }

    inline PolygonIterator<PolygonOrder> EndPolygons() {
        return PolygonIterator<PolygonOrder>(positions, normals, tangents, uvs, colors, polygons, polygons.size());
    }

    inline ConstPolygonIterator<PolygonOrder> EndPolygons() const {
        return ConstPolygonIterator<PolygonOrder>(positions, normals, tangents, uvs, colors, polygons, polygons.size());
    }
};

template<size_t PolygonOrder>
MeshDraft<PolygonOrder>::MeshDraft(size_t numVerticesGuess, size_t numPolygonsGuess) {
    positions.reserve(numVerticesGuess);
    normals.reserve(numVerticesGuess);
    tangents.reserve(numVerticesGuess);
    uvs.reserve(numVerticesGuess);
    colors.reserve(numVerticesGuess);

    polygons.reserve(numPolygonsGuess);
}

template<size_t PolygonOrder>
void MeshDraft<PolygonOrder>::GenerateTangents() {
    LOG_INFO(cfl::procgen::MeshDraft::GenerateTangents) << "¡¡Unimplemented!!";
}

template<size_t PolygonOrder>
void MeshDraft<PolygonOrder>::AddQuad(vec3 origin, vec3 width, vec3 height) {
    vec3 normal = glm::normalize(glm::cross(height, width));
    AddQuad(origin, origin + height, origin + height + width, origin + width,
            normal, normal, normal, normal);
}

template<size_t PolygonOrder>
void MeshDraft<PolygonOrder>::AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3) {
    vec3 normal = glm::normalize(glm::cross(vertex1 - vertex0, vertex2 - vertex0));
    AddQuad(vertex0, vertex1, vertex2, vertex3,
            normal, normal, normal, normal);
}

template<size_t PolygonOrder>
void MeshDraft<PolygonOrder>::AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3,
                                      vec3 normal) {
    AddQuad(vertex0, vertex1, vertex2, vertex3,
            normal, normal, normal, normal);
}

template<size_t PolygonOrder>
void MeshDraft<PolygonOrder>::AddQuad(vec3 origin, vec3 width, vec3 height,
                                      vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3) {
    uvs.emplace_back(uv0);
    uvs.emplace_back(uv1);
    uvs.emplace_back(uv2);
    uvs.emplace_back(uv3);
    AddQuad(origin, width, height);
}

template<size_t PolygonOrder>
void
MeshDraft<PolygonOrder>::AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3,
                                 vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3) {
    uvs.emplace_back(uv0);
    uvs.emplace_back(uv1);
    uvs.emplace_back(uv2);
    uvs.emplace_back(uv3);
    AddQuad(vertex0, vertex1, vertex2, vertex3);
}

template<size_t PolygonOrder>
void MeshDraft<PolygonOrder>::AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3,
                                      vec3 normal,
                                      vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3) {
    uvs.emplace_back(uv0);
    uvs.emplace_back(uv1);
    uvs.emplace_back(uv2);
    uvs.emplace_back(uv3);
    AddQuad(vertex0, vertex1, vertex2, vertex3, normal);
}

template<size_t PolygonOrder>
void MeshDraft<PolygonOrder>::AddQuad(vec3 vertex0, vec3 vertex1, vec3 vertex2, vec3 vertex3,
                                      vec3 normal0, vec3 normal1, vec3 normal2, vec3 normal3,
                                      vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3) {
    uvs.emplace_back(uv0);
    uvs.emplace_back(uv1);
    uvs.emplace_back(uv2);
    uvs.emplace_back(uv3);
    AddQuad(vertex0, vertex1, vertex2, vertex3, normal0, normal1, normal2, normal3);
}

template<size_t PolygonOrder>
bool MeshDraft<PolygonOrder>::ValidateVertices() const {
    auto const N = positions.size();
    if (normals.size() != N && normals.size() != 0) {
        LOG_ERROR(cfl::procgen::MeshDraft::ValidateVertices) << "normals.size() != positions.size(), " << normals.size()
                                                             << " vs. " << N;
        return false;
    }

    if (tangents.size() != N && tangents.size() != 0) {
        LOG_ERROR(cfl::procgen::MeshDraft::ValidateVertices) << "tangents.size() != positions.size(), "
                                                             << tangents.size()
                                                             << " vs. " << N;
        return false;
    }

    if (uvs.size() != N && uvs.size() != 0) {
        LOG_ERROR(cfl::procgen::MeshDraft::ValidateVertices) << "uvs.size() != positions.size(), " << uvs.size()
                                                             << " vs. " << N;
        return false;
    }

    if (colors.size() != N && colors.size() != 0) {
        LOG_ERROR(cfl::procgen::MeshDraft::ValidateVertices) << "colors.size() != positions.size(), " << colors.size()
                                                             << " vs. " << N;
        return false;
    }

    return true;
}

template<size_t PolygonOrder>
bool MeshDraft<PolygonOrder>::ValidatePolygonIndices() const {
    auto const N = positions.size();

    size_t count = 0;
    for (auto const &polygon : polygons) {
        for (uint i = 0; i < PolygonOrder; ++i) {
            auto const pi = polygon[i];

            if (pi >= N) {
                LOG_ERROR(cfl::procgen::MeshDraft::ValidateVertices) << "contains invalid vertex (" << pi
                                                                     << ") at position " << count;
                return false;
            }

            for (uint j = i + 1; j < PolygonOrder; ++j) {
                auto const pj = polygon[j];
                if (pj == pi) {
                    LOG_ERROR(cfl::procgen::MeshDraft::ValidateVertices) << "polygon (" << count
                                                                         << ") contains same vertex (" << pi
                                                                         << ") twice " << " (indices " << i
                                                                         << " and " << j << ").";
                    return false;
                }
            }
        }

        count++;
    }

    return true;
}
} // namespace procgen
} // namespace cfl
