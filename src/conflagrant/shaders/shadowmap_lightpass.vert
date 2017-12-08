#version 410

#include "common/Definitions.glsl"
#include "common/VertexAttributes.glsl"

out vec3 fIn_WorldPosition;
out vec2 fIn_TexCoord;

#include "common/Uniforms.glsl"

void main(void) {
    fIn_WorldPosition = vec3(M * vec4(vIn_Position, 1.0));
    gl_Position = P * V * vec4(fIn_WorldPosition, 1.0);
    fIn_TexCoord = vIn_TexCoord;
}
