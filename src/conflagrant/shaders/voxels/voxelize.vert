#version 450

#include "common/Definitions.glsl"
#include "common/VertexAttributes.glsl"

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 gIn_WorldPosition;
out mat3 gIn_WorldTBN;
out vec2 gIn_TexCoord;
out vec4 gIn_DirectionalLightSpacePositions[MAX_DIRECTIONALLIGHTS];

#include "common/DirectionalLight.glsl"

uniform DirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
uniform int numDirectionalLights = 0;

void main(void){
    vec4 worldPosition = M * vec4(vIn_Position, 1.0);
    gIn_WorldPosition = vec3(worldPosition);
    gIn_TexCoord = vIn_TexCoord;

    vec3 T = normalize(vec3(M * vec4(vIn_Tangent, 0.0)));
    vec3 N = normalize(vec3(M * vec4(vIn_Normal, 0.0)));
    vec3 B = normalize(vec3(M * vec4(vIn_Bitangent, 0.0)));

    gIn_WorldTBN = mat3(T, B, N);

    for (int i = 0; i < numDirectionalLights; ++i) {
        gIn_DirectionalLightSpacePositions[i] = directionalLights[i].VP * worldPosition;
    }

    // VP should be unneccessary since we explicitly set gl_Position in geometry shader anyway
#ifdef USE_PV
    gl_Position = P * V * vec4(gIn_WorldPosition, 1.0);
#else
    gl_Position = vec4(gIn_WorldPosition, 1.0);
#endif
}
