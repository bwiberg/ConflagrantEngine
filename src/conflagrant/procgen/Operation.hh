#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/procgen/QuadMeshDraft.hh>

#include <functional>

namespace cfl {
namespace procgen {
template<typename Iterator>
using Operation = std::function<void(Iterator)>;

template<typename VertexIterator>
using VertexOperation = std::function<void(VertexIterator)>;

template<typename PolygonIterator>
using PolygonOperation = std::function<void(PolygonIterator)>;

template<typename Iterator>
void Apply(Operation<Iterator> operation,
           Iterator begin, Iterator end) {
    while (begin != end) {
        operation(begin);
        ++begin;
    }
}
} // namespace procgen
} // namespace cfl
