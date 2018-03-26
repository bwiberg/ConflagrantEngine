#include "common/Random.glsl"
#include "common/noise/noise3D.glsl"
#define NOISE_FUNCTION_NAME snoise_grad
#include "common/noise/noise3Dgrad.glsl"

uniform vec3 EmitterDimensions = vec3(1, 0, 10);
uniform mat4 EmitterTransform = mat4(1);
uniform float EmitterMaxHorizontalSpeed = 0.2;
uniform float EmitterInitialFallSpeed = 0.18;

uniform float WindStrength = 0.2;
uniform float WindFrequency = 10;

uniform float MaxSpeed = 0.1;

void RespawnParticle(
    in int particleID,
    inout Particle p,
    in float time,
    in float timeDelta,
    in vec3 gravity) {

    vec3 seed = vec3(0.81693 * particleID, time, timeDelta);

    const vec3 nyOffset = vec3(4.54219, 17.125325, -5.771239);
    const vec3 nzOffset = nyOffset * vec3(0.513, 0.967, 1.671);

    float nx = snoise(seed);
    float ny = snoise(seed + nyOffset);
    float nz = snoise(seed + nzOffset);

    p.position.xyz = vec3(nx, ny, nz);

    p.position = (EmitterTransform * vec4(p.position, 1)).xyz;

    p.velocity = vec3(
        mod(0.612 * particleID * nx, EmitterMaxHorizontalSpeed),
        -EmitterInitialFallSpeed,
        mod(0.612 * particleID * ny, EmitterMaxHorizontalSpeed)
    );

    p.angle = 0;
    p.lifetime = 0;
}

void SimulateSnowParticle(
    in int particleID,
    inout Particle p,
    in float time,
    in float timeDelta,
    in vec3 gravity) {

    vec3 wind;
    float windStrength = snoise_grad(
        WindFrequency * p.position + mod(WindFrequency * time, 100),
        wind
    );
    wind.y = 0;

    p.velocity += WindStrength * wind * timeDelta;

    float speed = length(p.velocity.xz);
    if (speed > MaxSpeed) {
        float adjSpeed = clamp(speed, 0, MaxSpeed);
        p.velocity.xz = p.velocity.xz * adjSpeed / speed;
    }

    p.position += p.velocity * timeDelta;
    p.angle += angleSpeed * timeDelta;
    p.lifetime += timeDelta;

    if (time <= timeDelta) {
        RespawnParticle(particleID, p, time, timeDelta, gravity);
    }

    if (p.position.y < -0.1) {
        RespawnParticle(particleID, p, time, timeDelta, gravity);
    }

    if (p.lifetime > 30) {
        RespawnParticle(particleID, p, time, timeDelta, gravity);
    }

    vec3 voxelCoords = GetUnitCubeCoordinates(p.position, VoxelCenter, VoxelHalfDimensions);
    if (!IsWithinVoxelColume(voxelCoords))
        return;

    voxelCoords = GetNormalizedCoordinatesFromUnitCubeCoordinates(voxelCoords);

    vec4 v = textureLod(VoxelizedScene, voxelCoords, 0);
    float m = max(v.r, max(v.g, max(v.b, v.a)));

    if (m > 0) {
        RespawnParticle(particleID, p, time, timeDelta, gravity);
    }
}
