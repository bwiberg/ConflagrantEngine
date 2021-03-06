#version 410

#include "common/Definitions.glsl"
#include "common/VertexAttributes.glsl"

out mat3 fIn_WorldTBN;
out vec3 fIn_WorldPosition;
out vec2 fIn_TexCoord;
out vec4 fIn_DirectionalLightSpacePositions[MAX_DIRECTIONALLIGHTS];

#include "common/DirectionalLight.glsl"

uniform DirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
uniform int numDirectionalLights = 0;

#include "common/Uniforms.glsl"

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
