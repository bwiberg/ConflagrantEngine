/**
 * Inspired by http://alienryderflex.com/polygon_inset/
 */

#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/procgen/graph/Polygon.hh>
#include <conflagrant/procgen/graph/Edge.hh>
#include <conflagrant/math.hh>

#include <functional>

namespace cfl {
namespace procgen {
inline Edge ShiftEdge(Edge const &edge, vec3 const &center, float distance) {
    auto edge2center = center - Center(edge);
    auto centerWeight = math::Clamp(distance / glm::length(edge2center), 0.f, 1.f);

    return {edge.begin + centerWeight * edge2center,
            edge.end + centerWeight * edge2center};

}

using InsetDistanceFunctor = std::function<float(size_t)>;

template<size_t Order>
PolygonStrip<4> Inset(Polygon<Order> &polygon, InsetDistanceFunctor distanceFunctor) {
    auto const polygonCopy = polygon;

    PolygonStrip<4> strip;
    strip.reserve(Order);

    vec3 center{0};
    for (size_t i = 0; i < Order; ++i) {
        center += polygon[i];
    }
    center /= static_cast<float>(Order);

    std::array<Edge, Order> edges = EdgeLoop(polygon);
    for (size_t icurr = 0; icurr < Order; ++icurr) {
        size_t iprev = math::Repeat(icurr + Order - 1, 0, Order - 1);

        {
            auto eprev = ShiftEdge(edges[iprev], center, distanceFunctor(iprev));
            auto ecurr = ShiftEdge(edges[icurr], center, distanceFunctor(icurr));

            vec3 eprevClosest, ecurrClosest;
            if (!geometry::TryComputeClosestPoints(eprev.begin, Direction(eprev),
                                                   ecurr.begin, Direction(ecurr),
                                                   eprevClosest, ecurrClosest)) {
                LOG_INFO(cfl::procgen::Inset) << "failed to find common vertex of two inset-translated edges";
                continue;
            }

            polygon[icurr] = .5f * (eprevClosest + ecurrClosest);
        }
    }

    for (size_t icurr = 0; icurr < Order; ++icurr) {
        size_t inext = math::Repeat(icurr + 1, 0, Order - 1);
        strip.push_back({
                                polygonCopy[icurr],
                                polygonCopy[inext],
                                polygon[inext],
                                polygon[icurr]
                        });
    }

    return strip;
}

template<size_t Order>
PolygonStrip<4> Inset(Polygon<Order> &polygon, std::array<float, Order> const &distances) {
    return Inset<Order>(polygon, [&distances](size_t index) { return distances[index]; });
}

template<size_t Order>
PolygonStrip<4> Inset(Polygon<Order> &polygon, float const distance) {
    return Inset<Order>(polygon, [distance](size_t) { return distance; });
}
} // namespace procgen
} // namespace cfl
