#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/procgen/graph/Polygon.hh>

namespace cfl {
namespace procgen {
template<size_t Order>
PolygonStrip<4> Extrude(Polygon<Order> &polygon, float const offset) {
    PolygonStrip<4> strip;
    strip.reserve(Order);

    vec3 const voffset = offset * Normal(polygon);

    vec3 firstLower, firstUpper, secondLower, secondUpper;
    firstLower = polygon[Order - 1];
    firstUpper = polygon[Order - 1] + voffset;

    for (size_t i = 0; i < Order; ++i) {
        secondLower = polygon[i];
        secondUpper = polygon[i] + voffset;

        strip.push_back({
                                   firstLower,
                                   secondLower,
                                   secondUpper,
                                   firstUpper
                           });

        firstLower = secondLower;
        firstUpper = secondUpper;
    }

    for (size_t i = 0; i < Order; ++i) {
        polygon[i] += voffset;
    }

    return strip;
}
} // namespace procgen
} // namespace cfl
