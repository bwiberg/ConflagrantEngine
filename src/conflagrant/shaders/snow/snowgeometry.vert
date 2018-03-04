#version 450

#include "common/Definitions.glsl"
#include "common/VertexAttributes.glsl"

out vec3 gIn_WorldPosition;
out mat3 gIn_WorldTBN;
out vec2 gIn_TexCoord;

#include "common/Uniforms.glsl"

void main(void) {
    vec4 worldPosition = M * vec4(vIn_Position, 1.0);
    gIn_WorldPosition = vec3(worldPosition);
    gl_Position = P * V * vec4(gIn_WorldPosition, 1.0);
    gIn_TexCoord = vIn_TexCoord;

    vec3 T = normalize(vec3(M * vec4(vIn_Tangent, 0.0)));
    vec3 N = normalize(vec3(M * vec4(vIn_Normal, 0.0)));
    vec3 B = normalize(vec3(M * vec4(vIn_Bitangent, 0.0)));

    gIn_WorldTBN = mat3(T, B, N);
}
