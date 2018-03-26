#include "common/Unpack.glsl"

#define ImageLoad2D_vec3_float(_image, _coords, _vec3, _float) \
Unpack_vec3_float(imageLoad(_image, _coords), _vec3, _float)

#define ImageStore2D_vec3_float(_image, _coords, _vec3, _float) \
    imageStore(_image, _coords, vec4(_vec3, _float))
