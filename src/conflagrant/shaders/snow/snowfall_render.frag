#version 410

#include "common/Definitions.glsl"
#include "common/SurfaceInfo.glsl"
#include "common/Uniforms.glsl"
#include "common/DirectionalLight.glsl"
#include "common/Lighting.glsl"

uniform sampler2D SceneDepth;
uniform vec3 EyePos;

uniform sampler3D AmbientSceneLight;
uniform vec3 VoxelHalfDimensions;
uniform vec3 VoxelCenter;

uniform DirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
uniform int numDirectionalLights = 0;

flat in int fIn_ParticleID;
in vec3 fIn_WorldPositionCenter;
in vec3 fIn_WorldPosition;
in vec3 fIn_WorldVelocity;
in float fIn_Angle;
in float fIn_Lifetime;
in vec2 fIn_TexCoord;

out vec4 OutColor;

#include "common/noise/noise2D.glsl"

float nnoise(vec2 coords) {
    return 0.5 + 0.5 * snoise(coords);
}

#define FRACTAL_NOISE_FUNCTION2 nnoise
#include "common/noise/fractal.glsl"
#undef FRACTAL_NOISE_FUNCTION2

uniform int SnowflakeNoiseOctaves = 3;
uniform float SnowflakeNoiseScale = 1.0;
uniform float SnowflakeNoiseAmplitude = 10.0;
uniform float SnowflakeNoiseLacunarity = 2.0;
uniform float SnowflakeNoisePersistence = 0.8;

float snow_noise() {
    vec2 offset = vec2(fIn_ParticleID * 0.169, fIn_ParticleID * 0.713);
    float ns = fractal_noise(fIn_TexCoord + offset,
                             SnowflakeNoiseOctaves,
                             SnowflakeNoiseScale,
                             SnowflakeNoiseAmplitude,
                             SnowflakeNoiseLacunarity,
                             SnowflakeNoisePersistence);
    return max(0.2, 0.14 * ns);
}

#define VOXEL_NO_COMPUTE
#include "voxels/common/util.glsl"
#undef VOXEL_NO_COMPUTE

void main(void) {
    float r = length(2 * fIn_TexCoord - 1);
    float alpha = smoothstep(0, 1, 1 - r);

    vec2 screenTexCoord = gl_FragCoord.xy / ScreenSize;

    float depth = gl_FragCoord.z;
    float sceneDepth = texture(SceneDepth, screenTexCoord).x;

    float depthFade = clamp(SNOW_DEPTH_BLEND_FACTOR * abs(sceneDepth - depth), 0, 1);

    alpha *= depthFade;
    alpha *= snow_noise();

    SurfaceInfo surf;
    surf.WorldPosition = fIn_WorldPosition;
    surf.Diffuse = vec3(1);
    surf.Specular = 0;
    surf.Shininess = 0;
    surf.Radiance = 0;

    vec3 E = normalize(EyePos - surf.WorldPosition);

    vec3 color = vec3(0.5);

    vec3 voxelCoordinates = GetUnitCubeCoordinates(surf.WorldPosition, VoxelCenter, VoxelHalfDimensions);
    if (IsWithinVoxelColume(voxelCoordinates)) {
        voxelCoordinates = GetNormalizedCoordinatesFromUnitCubeCoordinates(voxelCoordinates);
        vec4 voxel = textureLod(AmbientSceneLight, voxelCoordinates, 3);

        color += 15 * voxel.rgb * voxel.a;
    }

    int i = 0;
    for (i = 0; i < numDirectionalLights; i++) {
        surf.Normal = directionalLights[i].direction;

        color += 2 * ApplyDirectionalLight(surf, directionalLights[i],
                                        directionalLights[i].VP * vec4(surf.WorldPosition, 1), E);
    }

    OutColor = vec4(0.6 * color, 0.5 * pow(alpha, 0.75));
}
