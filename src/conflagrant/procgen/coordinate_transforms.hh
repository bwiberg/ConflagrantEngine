#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
namespace procgen {
template <typename T>
T Flatten(T const& ix, T const& iy,
          T const& dx) {
    return iy * dx + ix;
};

template <typename T, glm::precision P>
T Flatten(glm::tvec2<T, P> const &index, glm::tvec2<T, P> const &dimensions) {
    return Flatten(index.x, index.y, dimensions.x);
};

template <typename T>
T Flatten(T const& ix, T const& iy,T const& iz,
          T const& dx, T const& dy) {
    return (ix  * dy + iy) * dx + ix;
};

template <typename T, glm::precision P>
T Flatten(glm::tvec3<T, P> const &index, glm::tvec3<T, P> const &dimensions) {
    return Flatten(index.x, index.y, index.z, dimensions.x, dimensions.y);
};

template <typename T>
void Expand(T const& index, T const &dx, T &outx, T & outy) {
    outy = index / dx;
    outx = index - outy;
}

template <typename T, glm::precision P>
glm::tvec2<T, P> Expand(T const& index, glm::tvec2<T, P> const &dimensions) {
    glm::tvec2<T, P> expanded;
    Expand(index, dimensions.x, expanded);
    return expanded;
}

template <typename T>
void Expand(T const& index, T const& dx, T const& dy, T &outx, T &outy, T &outz) {
    T indexCopy(index);

    outz = index / (dx * dy);
    indexCopy -= outz;
    outy = indexCopy / dx;
    outx = indexCopy - outy;
}

template <typename T, glm::precision P>
glm::tvec3<T, P> Expand(T const& index, glm::tvec3<T, P> const &dimensions) {
    glm::tvec3<T, P> expanded;
    Expand(index, dimensions.x, dimensions.y, expanded.x, expanded.y, expanded.z);
    return expanded;
}
} // namespace procgen
} // namespace cfl
