#version 410

#define MAX_DIRECTIONALLIGHTS 16

layout (location = 0) in vec3 vIn_Position;
layout (location = 1) in vec3 vIn_Normal;
layout (location = 2) in vec3 vIn_Tangent;
layout (location = 3) in vec3 vIn_Bitangent;
layout (location = 4) in vec2 vIn_TexCoord;

out mat3 fIn_WorldTBN;
out vec3 fIn_WorldPosition;
out vec2 fIn_TexCoord;
out vec4 fIn_DirectionalLightSpacePositions[MAX_DIRECTIONALLIGHTS];

struct DirectionalLight {
    vec3 direction;
    float intensity;
    vec3 color;

    int hasShadowMap;
    sampler2D shadowMap;
    mat4 VP;
};

uniform DirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
uniform int numDirectionalLights = 0;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform float time;

void main(void) {
    vec4 worldPosition = M * vec4(vIn_Position, 1.0);
    fIn_WorldPosition = vec3(worldPosition);
    gl_Position = P * V * vec4(fIn_WorldPosition, 1.0);
    fIn_TexCoord = vIn_TexCoord;

    vec3 T = normalize(vec3(M * vec4(vIn_Tangent, 0.0)));
    vec3 N = normalize(vec3(M * vec4(vIn_Normal, 0.0)));
    vec3 B = normalize(vec3(M * vec4(vIn_Bitangent, 0.0)));

    fIn_WorldTBN = mat3(T, B, N);

    for (int i = 0; i < numDirectionalLights; ++i) {
        fIn_DirectionalLightSpacePositions[i] = directionalLights[i].VP * worldPosition;
    }
}
