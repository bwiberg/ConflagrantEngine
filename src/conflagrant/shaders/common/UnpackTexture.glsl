#include "common/Unpack.glsl"

void Texture_vec3_float(sampler2D sampler, vec2 texCoords, out vec3 _vec3, out float _float) {
    Unpack_vec3_float(texture(sampler, texCoords), _vec3, _float);
}
