#version 410

layout (location = 0) in vec3 vIn_Position;
layout (location = 1) in vec3 vIn_Normal;
layout (location = 2) in vec3 vIn_Tangent;
layout (location = 3) in vec3 vIn_Bitangent;
layout (location = 4) in vec2 vIn_TexCoord;

out vec2 fIn_TexCoord;

void main(void) {
    gl_Position = vec4(vIn_Position, 1);
    fIn_TexCoord = vIn_TexCoord;
}
