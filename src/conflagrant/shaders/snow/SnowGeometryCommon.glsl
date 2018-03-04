uniform float SnowLowerUpness = 0.3;
uniform float SnowUpperUpness = 0.55;

uniform float SnowinessMin = 0.4;

uniform float SnowMinHeight = 0.01;
uniform float SnowMaxHeight = 0.04;

uniform vec3 SnowDiffuse = vec3(1);
uniform float SnowSpecular = 0;
uniform float SnowShininess = 0;

uniform int SnowNoiseOctaves = 1;
uniform float SnowNoiseScale = 1.0;
uniform float SnowNoiseAmplitude = 1.0;
uniform float SnowNoiseLacunarity = 2.0;
uniform float SnowNoisePersistence = 0.8;

#include "common/noise/noise2D.glsl"

float nnoise(vec2 coords) {
    return 1;
}

#define FRACTAL_NOISE_FUNCTION2 nnoise
#include "common/noise/fractal.glsl"
#undef FRACTAL_NOISE_FUNCTION2

float CalcSnowNoise(vec3 worldPosition) {
    return max(0.0, fractal_noise(worldPosition.xz,
                          SnowNoiseOctaves,
                          SnowNoiseScale,
                          SnowNoiseAmplitude,
                          SnowNoiseLacunarity,
                          SnowNoisePersistence));
}

float CalcSnowiness(vec3 normal) {
    float upness = max(0, dot(vec3(0, 1, 0), normal));
    return smoothstep(SnowLowerUpness, SnowUpperUpness, upness);
}
