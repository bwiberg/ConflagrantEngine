#version 410

layout (location = 0) in vec3 vIn_Position;
layout (location = 1) in vec3 vIn_Normal;
layout (location = 2) in vec3 vIn_Tangent;
layout (location = 3) in vec3 vIn_Bitangent;
layout (location = 4) in vec2 vIn_TexCoord;

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
