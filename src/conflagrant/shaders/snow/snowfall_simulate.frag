#version 410

#include "common/Definitions.glsl"
#include "common/UnpackTexture.glsl"

in vec2 fIn_TexCoord;

uniform sampler2D InPositionsAngles;
uniform sampler2D InVelocitiesLifetimes;
uniform int Count;
uniform int MaxCount;

uniform float time;
uniform float timeDelta;

uniform vec3 gravity = vec3(0, -1, 0);
uniform float angleSpeed = 1;

layout (location = 0) out vec4 OutPositionAngle;
layout (location = 1) out vec4 OutVelocityLifetime;

#define USE_TEXTURE
#include "snow/SnowParticle.glsl"
#include "snow/SimulateSnowParticle.glsl"

void main(void) {
    int particleID = int(fIn_TexCoord.s * MaxCount);

    Particle p = LoadParticle(fIn_TexCoord);

    SimulateSnowParticle(particleID, p, time, timeDelta, gravity);

    StoreParticle(p);
}
