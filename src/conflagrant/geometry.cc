#include "geometry.hh"

namespace cfl {
namespace geometry {
vec3 const Forward = vec3(0.f, 0.f, 1.f);
vec3 const Backward = -Forward;

vec3 const Right = vec3(1.f, 0.f, 0.f);
vec3 const Left = -Right;

vec3 const Up = vec3(0.f, 1.f, 0.f);
vec3 const Down = -Up;

mat3 const Identity3 = mat3(1);
mat4 const Identity4 = mat4(1);
} // namespace geometry
} // namespace cfl
