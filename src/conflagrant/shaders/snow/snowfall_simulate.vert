#version 410

#include "common/Definitions.glsl"
#include "common/VertexAttributes.glsl"

out vec2 fIn_TexCoord;

void main(void) {
    gl_Position = vec4(vIn_Position, 1.0);
    fIn_TexCoord = vec2(vIn_TexCoord.s, 1 - vIn_TexCoord.t);
}
