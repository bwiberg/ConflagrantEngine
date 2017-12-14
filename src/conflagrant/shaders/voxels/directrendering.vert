#version 410

#include "common/Definitions.glsl"
#include "common/VertexAttributes.glsl"

out vec3 fIn_RayOrigin;
out vec3 fIn_RayDirection;

uniform mat4 InverseVP;
uniform vec3 EyePos;

void main(void) {
    vec4 worldPosition = InverseVP * vec4(vIn_Position, 1.0);

    fIn_RayOrigin = worldPosition.xyz / worldPosition.w;
    fIn_RayDirection = normalize(fIn_RayOrigin - EyePos);

    gl_Position = vec4(vIn_Position, 1);
}
