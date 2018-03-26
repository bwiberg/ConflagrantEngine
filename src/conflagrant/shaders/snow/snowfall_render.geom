#version 450

#include "common/Definitions.glsl"
#include "common/Random.glsl"

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

#include "common/Uniforms.glsl"

uniform vec3 EyePos;
uniform float radius = 1;

flat in int gIn_ParticleID[1];
in vec3 gIn_WorldPosition[1];
in vec3 gIn_WorldVelocity[1];
in float gIn_Angle[1];
in float gIn_Lifetime[1];

flat out int fIn_ParticleID;
out vec3 fIn_WorldPositionCenter;
out vec3 fIn_WorldPosition;
out vec3 fIn_WorldVelocity;
out float fIn_Angle;
out float fIn_Lifetime;
out vec2 fIn_TexCoord;

// vertex ordering:
// 1---3
// |\  |
// | \ |
// |  \|
// 0---2
const vec2 Offsets[4] = vec2[4](
    vec2(0, 0),
    vec2(0, 1),
    vec2(1, 0),
    vec2(1, 1)
);

void main(void) {
    fIn_ParticleID = gIn_ParticleID[0];

    fIn_WorldPositionCenter = gIn_WorldPosition[0];
    fIn_WorldVelocity = gIn_WorldVelocity[0];
    fIn_Angle = gIn_Angle[0];
    fIn_Lifetime = gIn_Lifetime[0];

    vec3 eyeToParticle = gIn_WorldPosition[0] - EyePos;

    float cosa = cos(fIn_Angle);
    float sina = sin(fIn_Angle);
    mat3 rot;
    rot[0] = vec3(cosa, -sina, 0);
    rot[1] = vec3(sina, cosa, 0);
    rot[2] = vec3(0, 0, 1);

    mat3 M = inverse(mat3(V)) * rot;

    float r = radius * (SNOW_MIN_RADIUS + mod(random(vec4(fIn_ParticleID)), 1 - SNOW_MIN_RADIUS));

    for (int i = 0; i < 4; ++i) {
        fIn_TexCoord = Offsets[i];
        fIn_WorldPosition = fIn_WorldPositionCenter + M * vec3(r * (2 * Offsets[i] - 1), 0);
        gl_Position = P * V * vec4(fIn_WorldPosition, 1.0);
        EmitVertex();
    }

    EndPrimitive();
}
