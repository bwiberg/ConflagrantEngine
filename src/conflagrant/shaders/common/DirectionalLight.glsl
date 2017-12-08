#include "common/Definitions.glsl"

struct DirectionalLight {
    vec3 direction;
    float intensity;
    vec3 color;

    int hasShadowMap;
#if USE_SAMPLER2DSHADOW == 1
    sampler2DShadow shadowMap;
#else
    sampler2D shadowMap;
#endif
    mat4 VP;
};
