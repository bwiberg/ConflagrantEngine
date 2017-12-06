#include "geometry.hh"

namespace cfl {
namespace geometry {
vec3 const Forward = vec3(0.f, 0.f, 1.f);
vec3 const Backward = -Forward;

vec3 const Right = vec3(1.f, 0.f, 0.f);
vec3 const Left = -Right;

vec3 const Up = vec3(0.f, 1.f, 0.f);
vec3 const Down = -Up;
} // namespace geometry
} // namespace cfl
