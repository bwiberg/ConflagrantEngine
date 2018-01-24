#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/procgen/graph/Polygon.hh>
#include <conflagrant/math.hh>

namespace cfl {
namespace procgen {
struct Edge {
    vec3 begin, end;
};

template<size_t PolygonOrder>
std::array<Edge, PolygonOrder> EdgeLoop(Polygon<PolygonOrder> const& polygon) {
    std::array<Edge, PolygonOrder> edges;

    for (size_t i = 0; i < PolygonOrder; ++i) {
        edges[i] = {polygon[i], polygon[math::Repeat(i + 1, 0, PolygonOrder - 1)]};
    }

    return edges;
};

inline vec3 Center(Edge const& edge) {
    return .5f * (edge.begin + edge.end);
}

inline vec3 Direction(Edge const& edge) {
    return glm::normalize(edge.end - edge.begin);
}
} // namespace procgen
} // namespace cfl
