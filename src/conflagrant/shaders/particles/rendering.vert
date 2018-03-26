#version 410

#include "common/Definitions.glsl"

layout (location = 0) in vec3 vIn_Position;
layout (location = 1) in vec3 vIn_Velocity;
layout (location = 2) in float vIn_Rotation;

out vec3 gIn_WorldPosition;
out vec3 gIn_WorldVelocity;
out float gIn_Rotation;

#include "common/Uniforms.glsl"

void main(void) {
    gIn_WorldPosition = vIn_Position;
    gIn_WorldVelocity = vIn_Velocity;
    gIn_Rotation = vIn_Rotation;

    gl_Position = vec4(gIn_WorldPosition, 1.0);
}
