#version 410

#include "common/Definitions.glsl"
#include "common/UnpackTexture.glsl"
#include "common/VertexAttributes.glsl"
#include "common/Uniforms.glsl"

uniform sampler2D InPositionsAngles;
uniform sampler2D InVelocitiesLifetimes;
uniform int Count;
uniform int MaxCount;

#define ID gl_InstanceID

flat out int gIn_ParticleID;
out vec3 gIn_WorldPosition;
out vec3 gIn_WorldVelocity;
out float gIn_Angle;
out float gIn_Lifetime;

void main(void) {
    gIn_ParticleID = ID;

    vec2 texCoord = vec2(float(ID) / MaxCount, 0);

    Texture_vec3_float(InPositionsAngles,     texCoord, gIn_WorldPosition, gIn_Angle);
    Texture_vec3_float(InVelocitiesLifetimes, texCoord, gIn_WorldVelocity, gIn_Lifetime);

    gl_Position = vec4(gIn_WorldPosition, 1.0);
}
