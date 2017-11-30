#version 410

layout (location = 0) in vec3 vIn_Position;
layout (location = 1) in vec3 vIn_Normal;
layout (location = 2) in vec3 vIn_Tangent;
layout (location = 3) in vec3 vIn_Bitangent;
layout (location = 4) in vec2 vIn_TexCoord;

out mat3 fIn_WorldTBN;
out vec3 fIn_WorldPosition;
out vec2 fIn_TexCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform float time;

void main(void) {
    fIn_WorldPosition = vec3(M * vec4(vIn_Position, 1.0));
    gl_Position = P * V * vec4(fIn_WorldPosition, 1.0);
    fIn_TexCoord = vIn_TexCoord;

    vec3 T = normalize(vec3(M * vec4(vIn_Tangent,   0.0)));
    vec3 N = normalize(vec3(M * vec4(vIn_Normal,    0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(T, N);

    fIn_WorldTBN = transpose(inverse(mat3(M))) * mat3(T, B, N);
}
