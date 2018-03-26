#version 450

#include "common/Definitions.glsl"

in vec3 fIn_WorldPosition;
in mat3 fIn_WorldTBN;
in vec2 fIn_TexCoord;
in float fIn_Snowiness;

#include "common/Material.glsl"
uniform Material material;

uniform float time;
uniform float timeSnowStart;
uniform vec3 EyePos;

layout (location = 0) out vec4 GPositionRadiance;
layout (location = 1) out vec4 GNormalShininess;
layout (location = 2) out vec4 GAlbedoSpecular;
layout (location = 3) out float GDepth;

#include "snow/SnowGeometryCommon.glsl"

void main(void) {
    float snow = CalcSnowNoise(fIn_WorldPosition, time - timeSnowStart) * fIn_Snowiness;

    vec4 diffuse = GetPropertyColor(material.diffuse, fIn_TexCoord);
    if (diffuse.a < 1.0 / 255) {
        // early out if surface is fully transparent
        discard;
    }

    vec3 N = fIn_WorldTBN[2];
    if (material.hasNormalMap != 0) {
        N = 2.0 * texture(material.normalMap, 1 - fIn_TexCoord).rgb - vec3(1.0);
        N = normalize(fIn_WorldTBN * N);
    }

    vec3 specular = GetPropertyColor(material.specular, fIn_TexCoord).rgb;
    float shininess = max(1, material.shininess);

    // write to GBuffer

    GPositionRadiance.xyz = fIn_WorldPosition;
    GPositionRadiance.w = (1 - snow) * material.radiance / VCT_RADIANCE_MAX;

    GNormalShininess.xyz = normalize((1 - snow) * N + snow * fIn_WorldTBN[2]);
    GNormalShininess.w = shininess + snow * SnowShininess;

    GAlbedoSpecular.rgb = (1 - snow) * diffuse.rgb + snow * SnowDiffuse;
    GAlbedoSpecular.a = (1 - snow) * max(max(specular.r, specular.g), specular.b);

    GDepth = gl_FragCoord.z;
}
