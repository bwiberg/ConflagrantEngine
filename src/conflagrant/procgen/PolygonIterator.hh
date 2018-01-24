#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
namespace procgen {
using std::vector;

template<size_t PolygonOrder>
class PolygonIterator {
    size_t index;

    vector<vec3> &positions;
    vector<vec3> &normals;
    vector<vec3> &tangents;
    vector<vec2> &uvs;
    vector<vec4> &colors;

    vector<std::array<uint, PolygonOrder>> &polygons;

public:
    class PolygonVertexIterator {
        PolygonIterator &polygonIterator;

        size_t polygonIndex, vertexIndex;

    public:
        inline PolygonVertexIterator(PolygonIterator &polygonIterator,
                                     size_t polygonIndex,
                                     size_t vertexIndex)
                : polygonIterator(polygonIterator),
                  polygonIndex(polygonIndex),
                  vertexIndex(vertexIndex) {}

        inline PolygonVertexIterator operator++() {
            PolygonVertexIterator i = *this;
            vertexIndex++;
            return i;
        }

        inline PolygonVertexIterator operator++(int) {
            vertexIndex++;
            return *this;
        }

        inline bool operator==(PolygonVertexIterator const &rhs) {
            return polygonIterator == rhs.polygonIterator &&
                   polygonIndex == rhs.polygonIndex &&
                   vertexIndex == rhs.vertexIndex;
        }

        inline bool operator!=(PolygonVertexIterator const &rhs) {
            return !(*this == rhs);
        }
    };

    inline PolygonIterator(vector<vec3> &positions,
                           vector<vec3> &normals,
                           vector<vec3> &tangents,
                           vector<vec2> &uvs,
                           vector<vec4> &colors,
                           vector<std::array<uint, PolygonOrder>> &polygons,
                           size_t index = 0)
            : index(index),
              positions(positions),
              normals(normals),
              tangents(tangents),
              uvs(uvs),
              colors(colors),
              polygons(polygons) {}

    inline PolygonIterator(PolygonIterator const &other)
            : index(other.index),
              positions(other.positions),
              normals(other.normals),
              tangents(other.tangents),
              uvs(other.uvs),
              colors(other.colors),
              polygons(other.polygons) {}

    inline PolygonIterator operator++() {
        PolygonIterator i = *this;
        index++;
        return i;
    }

    inline PolygonIterator operator++(int) {
        index++;
        return *this;
    }

    inline bool operator==(PolygonIterator const &rhs) { return index == rhs.index; }

    inline bool operator!=(PolygonIterator const &rhs) { return index != rhs.index; }

    PolygonVertexIterator operator*() {
        return PolygonVertexIterator(*this, index, 0);
    }

    inline size_t Index() const {
        return index;
    }
};

template<size_t PolygonOrder>
class ConstPolygonIterator {
    size_t index;

    vector<vec3> const &positions;
    vector<vec3> const &normals;
    vector<vec3> const &tangents;
    vector<vec2> const &uvs;
    vector<vec4> const &colors;

    vector<std::array<uint, PolygonOrder>> const &polygons;

public:
    class ConstPolygonVertexIterator {
        ConstPolygonIterator const &polygonIterator;

        size_t polygonIndex, vertexIndex;

    public:
        inline ConstPolygonVertexIterator(ConstPolygonIterator const &polygonIterator,
                                          size_t polygonIndex,
                                          size_t vertexIndex)
                : polygonIterator(polygonIterator),
                  polygonIndex(polygonIndex),
                  vertexIndex(vertexIndex) {}

        inline ConstPolygonVertexIterator operator++() {
            ConstPolygonVertexIterator i = *this;
            vertexIndex++;
            return i;
        }

        inline ConstPolygonVertexIterator operator++(int) {
            vertexIndex++;
            return *this;
        }

        inline bool operator==(ConstPolygonVertexIterator const &rhs) {
            return polygonIterator == rhs.polygonIterator &&
                   polygonIndex == rhs.polygonIndex &&
                   vertexIndex == rhs.vertexIndex;
        }

        inline bool operator!=(ConstPolygonVertexIterator const &rhs) {
            return !(*this == rhs);
        }
    };

    inline ConstPolygonIterator(vector<vec3> const &positions,
                                vector<vec3> const &normals,
                                vector<vec3> const &tangents,
                                vector<vec2> const &uvs,
                                vector<vec4> const &colors,
                                vector<std::array<uint, PolygonOrder>> const &polygons,
                                size_t index = 0)
            : index(index),
              positions(positions),
              normals(normals),
              tangents(tangents),
              uvs(uvs),
              colors(colors),
              polygons(polygons) {}

    inline ConstPolygonIterator(ConstPolygonIterator const &other)
            : index(other.index),
              positions(other.positions),
              normals(other.normals),
              tangents(other.tangents),
              uvs(other.uvs),
              colors(other.colors),
              polygons(other.polygons) {}

    inline ConstPolygonIterator operator++() {
        ConstPolygonIterator i = *this;
        index++;
        return i;
    }

    inline ConstPolygonIterator operator++(int) {
        index++;
        return *this;
    }

    inline bool operator==(ConstPolygonIterator const &rhs) { return index == rhs.index; }

    inline bool operator!=(ConstPolygonIterator const &rhs) { return index != rhs.index; }

    ConstPolygonVertexIterator operator*() {
        return ConstPolygonVertexIterator(*this, index, 0);
    }

    inline size_t Index() const {
        return index;
    }
};
} // namespace procgen
} // namespace cfl
