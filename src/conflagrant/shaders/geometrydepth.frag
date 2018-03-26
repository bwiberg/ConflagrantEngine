#version 410

#include "common/Definitions.glsl"

in vec3 fIn_WorldPosition;
in mat3 fIn_WorldTBN;
in vec2 fIn_TexCoord;

#include "common/Material.glsl"
uniform Material material;

uniform float time;
uniform vec3 EyePos;

layout (location = 0) out float OutDepth;

void main(void) {
    vec4 diffuse = GetPropertyColor(material.diffuse, fIn_TexCoord);
    if (diffuse.a < 1.0 / 255) {
        // early out if surface is fully transparent
        discard;
    }

    OutDepth = gl_FragCoord.z;
}
