#version 410

#include "common/Definitions.glsl"
#include "common/VertexAttributes.glsl"

out vec2 fIn_TexCoord;

void main(void) {
    gl_Position = vec4(vIn_Position, 1);
    fIn_TexCoord = vIn_TexCoord;
}
