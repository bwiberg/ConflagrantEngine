#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/geometry.hh>
#include <conflagrant/procgen/TriMeshDraft.hh>
#include <conflagrant/procgen/QuadMeshDraft.hh>

#include <algorithm>

namespace cfl {
namespace procgen {
template<size_t Order>
using Polygon = std::array<vec3, Order>;

inline Polygon<4> Quad() {
    static constexpr float upper = 1.f;
    static constexpr float lower = 0.f;

    return {vec3(lower, 0.f, lower),
            vec3(upper, 0.f, lower),
            vec3(upper, 0.f, upper),
            vec3(lower, 0.f, upper)};
}

template<size_t Order>
using PolygonStrip = std::vector<Polygon<Order>>;

template<size_t Order>
vec3 Normal(Polygon<Order> const& polygon) {
    static_assert(Order > 2);
    return glm::normalize(glm::cross(polygon[1] - polygon[0], polygon[2] - polygon[0]));
}

template<size_t Order>
void Flip(Polygon<Order> &polygon) {
    std::reverse(std::next(polygon.begin()), polygon.end());
}

template<size_t Order>
TriMeshDraft ToMeshDraft(Polygon<Order> const &polygon) {
    static constexpr size_t TrisPerPolygon = Order - 2;

    TriMeshDraft draft;

    for (size_t i = 0; i < TrisPerPolygon; ++i) {
        draft.AddTriangle(polygon[0],
                          polygon[1 + i],
                          polygon[2 + i]);
    }

    return draft;
};

template<size_t Order, typename PolygonContainer>
TriMeshDraft ToMeshDraft(PolygonContainer const &polygons) {
    static constexpr size_t TrisPerPolygon = Order - 2;

    TriMeshDraft draft;

    for (Polygon<Order> const &polygon : polygons) {
        for (size_t i = 0; i < TrisPerPolygon; ++i) {
            draft.AddTriangle(polygon[0],
                              polygon[1 + i],
                              polygon[2 + i]);
        }
    }

    return draft;
};
} // namespace procgen
} // namespace cfl
