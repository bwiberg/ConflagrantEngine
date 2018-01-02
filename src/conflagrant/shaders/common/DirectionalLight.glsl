#include "common/Definitions.glsl"
#include "common/Lighting.glsl"
#include "common/Random.glsl"

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

float ComputeVisibilityPCF(SAMPLER2DSHADOW shadowMap, vec3 projCoords, float bias) {
    float shadow = 0.0;
    vec3 pcfMult = vec3(1.0 / textureSize(shadowMap, 0), 0);
    for (int x = -PCF_RADIUS; x <= PCF_RADIUS; ++x) {
        for (int y = -PCF_RADIUS; y <= PCF_RADIUS; ++y) {
            float closestDepth = SAMPLE_SHADOWMAP(shadowMap, (projCoords + vec3(x, y, 0) * pcfMult), bias);
            shadow += projCoords.z - SHADOWMAP_BIAS < closestDepth ? 1.0 : 0.0;
        }
    }
    int samples = 2 * PCF_RADIUS + 1;
    return (shadow / (samples * samples));
}

float ComputeVisibilityPoisson(SAMPLER2DSHADOW shadowMap, vec3 projCoords, float bias, vec3 seed) {
    const vec2 poissonDisk[16] = vec2[](
       vec2( -0.94201624, -0.39906216 ),
       vec2( 0.94558609, -0.76890725 ),
       vec2( -0.094184101, -0.92938870 ),
       vec2( 0.34495938, 0.29387760 ),
       vec2( -0.91588581, 0.45771432 ),
       vec2( -0.81544232, -0.87912464 ),
       vec2( -0.38277543, 0.27676845 ),
       vec2( 0.97484398, 0.75648379 ),
       vec2( 0.44323325, -0.97511554 ),
       vec2( 0.53742981, -0.47373420 ),
       vec2( -0.26496911, -0.41893023 ),
       vec2( 0.79197514, 0.19090188 ),
       vec2( -0.24188840, 0.99706507 ),
       vec2( -0.81409955, 0.91437590 ),
       vec2( 0.19984126, 0.78641367 ),
       vec2( 0.14383161, -0.14100790 )
    );

    float shadow = 0.0;

    if (POISSON_CENTER_WEIGHT > 0) {
        float closestDepth = SAMPLE_SHADOWMAP(shadowMap, projCoords, bias);
        shadow += projCoords.z - SHADOWMAP_BIAS < closestDepth ? float(POISSON_CENTER_WEIGHT) : 0.0;
    }

    for (int i = 0; i < POISSON_SAMPLES; ++i) {
        int index = int(16.0 * random(vec4(seed, i))) % 16;
        float closestDepth = SAMPLE_SHADOWMAP(shadowMap, (projCoords + vec3(poissonDisk[index], 0) / POISSON_VALUE), bias);
        shadow += projCoords.z - SHADOWMAP_BIAS < closestDepth ? 1.0 : 0.0;
    }

    return shadow / (POISSON_SAMPLES + POISSON_CENTER_WEIGHT);
}

float ComputeVisibility(SurfaceInfo surf, DirectionalLight l, vec4 lightspacePosition) {
    float visibility;
    if (l.hasShadowMap == 0) {
        visibility = 1.0;
    } else {
        vec3 projCoords = lightspacePosition.xyz / lightspacePosition.w;
        projCoords = 0.5 * projCoords + 0.5;

        float bias = max(10 * SHADOWMAP_BIAS * (1.0 - dot(surf.Normal, l.direction)), SHADOWMAP_BIAS);

        float closestDepth = SAMPLE_SHADOWMAP(l.shadowMap, projCoords, bias);

#if SHADOWMAP_METHOD == SINGLE_SAMPLE
        visibility = projCoords.z - SHADOWMAP_BIAS < closestDepth ? 1.0 : 0.0;
#elif SHADOWMAP_METHOD == PCF
        visibility = ComputeVisibilityPCF(l.shadowMap, projCoords, bias);
#elif SHADOWMAP_METHOD == POISSON
        visibility = ComputeVisibilityPoisson(l.shadowMap, projCoords, bias, vec3(0, 0, 0));
#elif SHADOWMAP_METHOD == STRATIFIED_POISSON
        visibility = ComputeVisibilityPoisson(l.shadowMap, projCoords, bias, surf.WorldPosition);
#else
        visibility = 0.0;
#endif

        if (projCoords.z > 1.0) {
            visibility = 1.0;
        }
    }
    return visibility;
}

vec3 ApplyDirectionalLight(SurfaceInfo surf, DirectionalLight l, vec4 lightspacePosition, vec3 EyeDir) {
    return ComputeVisibility(surf, l, lightspacePosition)
        * ComputePhongShading(surf, l.direction, l.intensity * l.color, EyeDir);
}
