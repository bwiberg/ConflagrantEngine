#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

#include <cmath>

namespace cfl {
namespace math {
template<typename T>
inline T Clamp(T const& value, T const& minimum, T const& maximum) {
    if (value < minimum)
        return minimum;
    if (value > maximum)
        return maximum;
    return value;
}
} // namespace math
} // namespace cfl
