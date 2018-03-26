#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

#include "common/Definitions.glsl"

in vec3 gIn_WorldPosition;
in vec3 gIn_WorldVelocity;
in float gIn_Rotation;

out vec3 fIn_WorldPosition;
out vec3 fIn_WorldVelocity;
out mat3 fIn_WorldTBN;
out vec2 fIn_TexCoord;

uniform vec2 size;

uniform vec3 EyePos;
uniform vec3 EyeForward;
uniform vec3 EyeUp;
uniform vec3 EyeRight;

const vec3 positions[4] = vec3[](
    vec3(-1, -1, 0),
    vec3( 1, -1, 0),
    vec3(-1,  1, 0),
    vec3( 1,  1, 0)
);

const vec2 uvs[4] = vec2[](
    vec2(0, 0),
    vec2(1, 0),
    vec2(0, 1),
    vec2(1, 1)
);

void main(void) {
    const vec3 SIZE = vec3(size.x, size.y, 0);

    // construct particle coordinate system relative to eye
    fIn_WorldTBN[2] = normalize(EyePos - gIn_WorldPosition);
    fIn_WorldTBN[1] = EyeUp;
    fIn_WorldTBN[0] = - normalize(cross(fIn_WorldTBN[2], fIn_WorldTBN[1]));

    // emit quad (from 2 tris) aligned with eye coordinate system
    for (int i = 0; i < 4; ++i) {
        fIn_WorldPosition = gIn_WorldPosition + M * SIZE * positions[i];
        fIn_TexCoord = uvs[i];
        gl_Position = P * V * fIn_WorldPosition;

        EmitVertex();
    }

    EndPrimitive();
}
