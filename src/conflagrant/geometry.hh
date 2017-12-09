#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <type_traits>

namespace cfl {
namespace geometry {
enum class IntersectionType {
    ABOVE = 1,
    INTERSECTS = 0,
    BELOW = -1,
    OUTSIDE = static_cast<std::underlying_type<IntersectionType>::type>(ABOVE),
    INSIDE = static_cast<std::underlying_type<IntersectionType>::type>(BELOW)
};

extern vec3 const Forward, Backward, Right, Left, Up, Down;

struct Sphere;
struct Plane;
struct Frustum;

#define DECLARE_INTERSECT(type, name) IntersectionType ComputeIntersection(type const &name) const;
#define DEFINE_INTERSECT(type, itype, iname) inline IntersectionType type::ComputeIntersection(itype const& iname) const

struct Sphere {
    vec3 center;
    float radius;

    DECLARE_INTERSECT(Sphere, sphere);
};

struct Plane {
    vec3 center, normal;

    DECLARE_INTERSECT(Sphere, sphere);
};

struct Frustum {
    constexpr static size_t NumSides = 6;

    enum class Side {
        NEAR = 0,
        FAR = 1,
        TOP = 2,
        RIGHT = 3,
        BOTTOM = 4,
        LEFT = 5
    };

    /**
     * Frustum planes. Their normals point outward.
     * Order: [near, far, top, right, bottom, left.
     */
    std::array<Plane, NumSides> sides;

    inline Plane const &Get(Side side) const {
        auto index = static_cast<std::underlying_type<Side>::type>(side);
        assert(index >= 0 && index < NumSides);
        return sides[index];
    }

    DECLARE_INTERSECT(Sphere, sphere);
};

inline Sphere Transform(Sphere const &sphere, mat4 const &matrix, float radiusScale) {
    return Sphere{
            .center = vec3(matrix * vec4(sphere.center, 1.f)),
            .radius = radiusScale * sphere.radius
    };
}

inline Sphere operator*(mat4 const &transform, Sphere const &sphere) {
    return Sphere{
            .center = vec3(transform * vec4(sphere.center, 1.f)),
            .radius = sphere.radius
    };
}

inline Plane operator*(mat4 const &transform, Plane const &plane) {
    return Plane{
            .center = vec3(transform * vec4(plane.center, 1.f)),
            .normal = glm::normalize(vec3(transform * vec4(plane.normal, 0.f)))
    };
}

inline Frustum operator*(mat4 const &transform, Frustum const &frustum) {
    Frustum ret{};
    for (size_t i = 0; i < Frustum::NumSides; i++) {
        ret.sides[i] = transform * frustum.sides[i];
    }

    return ret;
}

DEFINE_INTERSECT(Sphere, Sphere, sphere) {
    auto d = sphere.center - center;

    auto distance2 = glm::dot(d, d);
    auto radiusSum = radius + sphere.radius;
    auto radiusSum2 = radiusSum * radiusSum;

    if (distance2 > radiusSum2) {
        return IntersectionType::OUTSIDE;
    }

    auto distance = glm::sqrt(distance2);
    if (distance + sphere.radius < radius) {
        return IntersectionType::INSIDE;
    }

    // todo check if 'this' is inside 'sphere'
    return IntersectionType::INTERSECTS;
}

DEFINE_INTERSECT(Plane, Sphere, sphere) {
    auto d = sphere.center - center;

    auto distance = glm::dot(normal, d);

    if (distance > sphere.radius) {
        return IntersectionType::OUTSIDE;
    }

    if (distance >= -sphere.radius) {
        return IntersectionType::INTERSECTS;
    }

    return IntersectionType::INSIDE;
}

DEFINE_INTERSECT(Frustum, Sphere, sphere) {
    // Algorithm:
    // if sphere is OUTSIDE any plane, then it is OUTSIDE the entire frustum as well
    // ONLY if sphere is INSIDE all frustum planes it is INSIDE the frustum itself
    // if sphere is OUTSIDE or INTERSECTS any frustum plane, then it is OUTSIDE/INTERSECTS
    // the entire frustum as well

    bool anyInside = false;
    for (uint i = 0; i < Frustum::NumSides; ++i) {
        auto type = sides[i].ComputeIntersection(sphere);
        if (type == IntersectionType::OUTSIDE) {
            return IntersectionType::OUTSIDE;
        }

        // INTERSECTS or INSIDE

        if (type == IntersectionType::INSIDE) {
            anyInside = true;
        }
    }

    return anyInside ? IntersectionType::INSIDE : IntersectionType::INTERSECTS;
}

#undef DECLARE_INTERSECT
#undef DEFINE_INTERSECT

}
} // namespace cfl
