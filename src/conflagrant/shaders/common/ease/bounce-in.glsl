#include "common/Constants.glsl"

float bounceIn(float t) {
  return 1.0 - bounceOut(1.0 - t);
}
