#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/procgen/graph/Polygon.hh>

namespace cfl {
namespace procgen {
enum class SplitAxis {
    HORIZONTAL,
    VERTICAL
};

inline PolygonStrip<4> Split(Polygon<4> const &polygon, size_t const N,
                             size_t beginLowerIndex,
                             size_t beginUpperIndex,
                             size_t endLowerIndex,
                             size_t endUpperIndex) {
    PolygonStrip<4> strip;
    strip.reserve(N);

    vec3 const BeginLower = polygon[beginLowerIndex];
    vec3 const BeginUpper = polygon[beginUpperIndex];

    vec3 const EndLower = polygon[endLowerIndex];
    vec3 const EndUpper = polygon[endUpperIndex];

    vec3 const DeltaLower = (EndLower - BeginLower) / static_cast<float>(N);
    vec3 const DeltaUpper = (EndUpper - BeginUpper) / static_cast<float>(N);

    for (size_t i = 0; i < N; ++i) {
        auto const t = static_cast<float>(i);
        strip.push_back({
                                BeginLower + DeltaLower * t,
                                BeginUpper + DeltaUpper * t,
                                BeginUpper + DeltaUpper * (t + 1),
                                BeginLower + DeltaLower * (t + 1)
                        });
    }

    return strip;
}

template<SplitAxis Axis>
PolygonStrip<4> Split(Polygon<4> const &polygon, size_t const N);

template<>
inline PolygonStrip<4> Split<SplitAxis::HORIZONTAL>(Polygon<4> const &polygon, size_t const N) {
    return Split(polygon, N, 0, 3, 1, 2);
}

template<>
inline PolygonStrip<4> Split<SplitAxis::VERTICAL>(Polygon<4> const &polygon, size_t const N) {
    return Split(polygon, N, 1, 0, 2, 3);
}

inline PolygonStrip<4> Split(Polygon<4> const &polygon, size_t const N, SplitAxis axis) {
    switch (axis) {
        case SplitAxis::HORIZONTAL:
            return Split<SplitAxis::HORIZONTAL>(polygon, N);
        case SplitAxis::VERTICAL:
        default:
            return Split<SplitAxis::VERTICAL>(polygon, N);
    }
}
} // namespace procgen
} // namespace cfl
