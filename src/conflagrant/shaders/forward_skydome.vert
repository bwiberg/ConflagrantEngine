#version 410

#include "common/Definitions.glsl"
#include "common/VertexAttributes.glsl"

out vec2 fIn_TexCoord;
out float fIn_ModelY;

uniform mat4 MVP;
uniform float time;

uniform float radius;

void main(void) {
    fIn_ModelY = vIn_Position.y;
    fIn_TexCoord = 0.5 * vIn_Position.xz + vec2(0.5, 0.5);
    gl_Position = MVP * vec4(radius * vIn_Position, 1.0);
}
