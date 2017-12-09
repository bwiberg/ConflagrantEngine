#version 410

#include "common/Definitions.glsl"
#include "common/VertexAttributes.glsl"

#include "common/Uniforms.glsl"

void main(void) {
    vec4 worldPosition = M * vec4(vIn_Position, 1.0);
    gl_Position = P * V * worldPosition;
}
