#include "common/Constants.glsl"

float sineIn(float t) {
  return sin((t - 1.0) * HALF_PI) + 1.0;
}
