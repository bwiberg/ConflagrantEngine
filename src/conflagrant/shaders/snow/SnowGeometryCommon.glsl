uniform float SnowLowerUpness = 0.3;
uniform float SnowUpperUpness = 0.8;

uniform float SnowinessMin = 0.4;

uniform float SnowMinHeight = 0.01;
uniform float SnowMaxHeight = 0.03;

uniform vec3 SnowDiffuse = vec3(1);
uniform float SnowSpecular = 0;
uniform float SnowShininess = 1;

uniform int SnowNoiseOctaves = 3;
uniform float SnowNoiseScale = 2.0;
uniform float SnowNoiseAmplitude = 0.9;
uniform float SnowNoiseLacunarity = 3.0;
uniform float SnowNoisePersistence = 0.5;

#include "common/noise/noise2D.glsl"

#define FRACTAL_NOISE_FUNCTION2 snoise
#include "common/noise/fractal.glsl"
#undef FRACTAL_NOISE_FUNCTION2

float CalcSnowNoise(vec3 worldPosition, float timeSinceStart) {
    float offset = min(0.5 * (time - timeSnowStart - 5), 2);
    return max(0.0, offset + fractal_noise(worldPosition.xz,
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
