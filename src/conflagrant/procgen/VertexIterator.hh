#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
namespace procgen {
using std::vector;

class VertexIterator {
    size_t index;

    vector<vec3> &positions;
    vector<vec3> &normals;
    vector<vec3> &tangents;
    vector<vec2> &uvs;
    vector<vec4> &colors;

public:
    inline VertexIterator(vector<vec3> &positions,
                          vector<vec3> &normals,
                          vector<vec3> &tangents,
                          vector<vec2> &uvs,
                          vector<vec4> &colors,
                          size_t index = 0)
            : index(index),
              positions(positions),
              normals(normals),
              tangents(tangents),
              uvs(uvs),
              colors(colors) {}

    inline VertexIterator(VertexIterator const &other)
            : index(other.index),
              positions(other.positions),
              normals(other.normals),
              tangents(other.tangents),
              uvs(other.uvs),
              colors(other.colors) {}

    inline VertexIterator operator++() {
        VertexIterator i = *this;
        index++;
        return i;
    }

    inline VertexIterator operator++(int) {
        index++;
        return *this;
    }

    inline bool operator==(VertexIterator const &rhs) {
        return index == rhs.index &&
               &positions == &rhs.positions &&
               &normals == &rhs.normals &&
               &tangents == &rhs.tangents &&
               &uvs == &rhs.uvs &&
               &colors == &rhs.colors;
    }

    inline bool operator!=(VertexIterator const &rhs) {
        return !(*this == rhs);
    }

    inline vec3 &Position() {
        return positions[index];
    }

    inline vec3 &Normal() {
        return normals[index];
    }

    inline vec3 &Tangent() {
        return tangents[index];
    }

    inline vec2 &UV() {
        return uvs[index];
    }

    inline vec4 &Color() {
        return colors[index];
    }

    inline size_t Index() const {
        return index;
    }
};

class ConstVertexIterator {
    size_t index;

    vector<vec3> const &positions;
    vector<vec3> const &normals;
    vector<vec3> const &tangents;
    vector<vec2> const &uvs;
    vector<vec4> const &colors;

public:
    inline ConstVertexIterator(vector<vec3> const &positions,
                               vector<vec3> const &normals,
                               vector<vec3> const &tangents,
                               vector<vec2> const &uvs,
                               vector<vec4> const &colors,
                               size_t index = 0)
            : index(index),
              positions(positions),
              normals(normals),
              tangents(tangents),
              uvs(uvs),
              colors(colors) {}

    inline ConstVertexIterator(ConstVertexIterator const &other)
            : index(other.index),
              positions(other.positions),
              normals(other.normals),
              tangents(other.tangents),
              uvs(other.uvs),
              colors(other.colors) {}

    inline ConstVertexIterator operator++() {
        ConstVertexIterator i = *this;
        index++;
        return i;
    }

    inline ConstVertexIterator operator++(int junk) {
        index++;
        return *this;
    }

    inline bool operator==(ConstVertexIterator const &rhs) {
        return index == rhs.index &&
               &positions == &rhs.positions &&
               &normals == &rhs.normals &&
               &tangents == &rhs.tangents &&
               &uvs == &rhs.uvs &&
               &colors == &rhs.colors;
    }

    inline bool operator!=(ConstVertexIterator const &rhs) {
        return !(*this == rhs);
    }

    inline vec3 const &Position() {
        return positions[index];
    }

    inline vec3 const &Normal() {
        return normals[index];
    }

    inline vec3 const &Tangent() {
        return tangents[index];
    }

    inline vec2 const &UV() {
        return uvs[index];
    }

    inline vec4 const &Color() {
        return colors[index];
    }

    inline size_t Index() const {
        return index;
    }
};
} // namespace procgen
} // namespace cfl
