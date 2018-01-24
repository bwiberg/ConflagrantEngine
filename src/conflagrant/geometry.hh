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
extern mat3 const Identity3;
extern mat4 const Identity4;

struct Sphere;
struct Plane;
struct Frustum;
struct AABB;
struct Ray;

struct Sphere {
    vec3 center;
    float radius;
};

struct Plane {
    vec3 center, normal;
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
};

struct AABB {
    vec3 origin, size;
};

struct Ray {
    vec3 origin, direction;
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

#define INTERSECT(LeftType, LeftName, RightType, RightName) \
IntersectionType Intersect(LeftType const& LeftName, RightType const& RightName); \
inline IntersectionType Intersect(RightType const& RightName, LeftType const& LeftName) {\
    return Intersect(LeftName, RightName); \
} \
inline IntersectionType Intersect(LeftType const& LeftName, RightType const& RightName)

#define INTERSECT_SELF(Type) \
inline IntersectionType Intersect(Type const& first, Type const& second)

INTERSECT_SELF(Sphere) {
    auto d = first.center - second.center;

    auto distance2 = glm::dot(d, d);
    auto radiusSum = second.radius + first.radius;
    auto radiusSum2 = radiusSum * radiusSum;

    if (distance2 > radiusSum2) {
        return IntersectionType::OUTSIDE;
    }

    auto distance = glm::sqrt(distance2);
    if (distance + first.radius < second.radius) {
        return IntersectionType::INSIDE;
    }

    // todo check if 'this' is inside 'sphere'
    return IntersectionType::INTERSECTS;
}

INTERSECT(Plane, plane, Sphere, sphere) {
    auto d = sphere.center - plane.center;

    auto distance = glm::dot(plane.normal, d);

    if (distance > sphere.radius) {
        return IntersectionType::OUTSIDE;
    }

    if (distance >= -sphere.radius) {
        return IntersectionType::INTERSECTS;
    }

    return IntersectionType::INSIDE;
}

INTERSECT(Frustum, frustum, Sphere, sphere) {
    // Algorithm:
    // if sphere is OUTSIDE any plane, then it is OUTSIDE the entire frustum as well
    // ONLY if sphere is INSIDE all frustum planes it is INSIDE the frustum itself
    // if sphere is OUTSIDE or INTERSECTS any frustum plane, then it is OUTSIDE/INTERSECTS
    // the entire frustum as well

    bool anyInside = false;
    for (uint i = 0; i < Frustum::NumSides; ++i) {
        auto type = Intersect(frustum.sides[i], sphere);
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

INTERSECT_SELF(AABB) {
#ifndef NDEBUG
    if (first.size.x >= 0 && first.size.y >= 0 && first.size.z >= 0 &&
        second.size.x >= 0 && second.size.y >= 0 && second.size.z >= 0) {
        LOG_ERROR(cfl::geometry::Intersect(AABB, AABB)) << "AABB's size can't have negative components";
        return IntersectionType::OUTSIDE;
    }
#endif

    return IntersectionType::OUTSIDE;
}

#undef INTERSECT
#undef INTERSECT_SELF

inline bool TryComputeClosestPoints(vec3 const &P0, vec3 const &u, vec3 const &Q0, vec3 const &v,
                                    vec3 &outP, vec3 &outQ, float const nonzero = 1e-5f) {
    vec3 const w0 = P0 - Q0;

    float const a = glm::dot(u, u);
    float const b = glm::dot(u, v);
    float const c = glm::dot(v, v);
    float const d = glm::dot(u, w0);
    float const e = glm::dot(v, w0);

    float const ac_b2 = a * c - b * b;
    if (ac_b2 < nonzero) {
        return false;
    }

    float const sc = ((b * e) - (c * d)) / ac_b2;
    float const tc = ((a * e) - (b * d)) / ac_b2;

    outP = P0 + sc * u;
    outQ = Q0 + tc * v;

    return true;
}

inline bool TryComputeClosestPoints(Ray const &P, Ray const &Q,
                                    vec3 &outP, vec3 &outQ, float const nonzero = 1e-5f) {
    return TryComputeClosestPoints(P.origin, P.direction, Q.origin, Q.direction,
                                   outP, outQ, nonzero);
}
}
} // namespace cfl
