#version 410

#include "common/Definitions.glsl"
#include "common/SurfaceInfo.glsl"

in vec2 fIn_TexCoord;

uniform sampler2D GInPositionRadiance;
uniform sampler2D GInNormalShininess;
uniform sampler2D GInAlbedoSpecular;

uniform float time;
uniform vec3 EyePos;

layout (location = 0) out vec4 GOutPositionRadiance;
layout (location = 1) out vec4 GOutNormalShininess;
layout (location = 2) out vec4 GOutAlbedoSpecular;

void TextureVec3AndFloat(sampler2D sampler, vec2 texCoords, out vec3 outvec3, out float outfloat) {
    vec4 value = texture(sampler, texCoords);
    outvec3 = value.xyz;
    outfloat = value.w;
}

void main(void) {
    SurfaceInfo surf;
    TextureVec3AndFloat(GInPositionRadiance, fIn_TexCoord, surf.WorldPosition, surf.Radiance);
    TextureVec3AndFloat(GInNormalShininess, fIn_TexCoord, surf.Normal, surf.Shininess);
    TextureVec3AndFloat(GInAlbedoSpecular, fIn_TexCoord, surf.Diffuse, surf.Specular);

    float upness = dot(surf.Normal, vec3(0.0, 1.0, 0.0));
    upness = max(0.0, upness);

    // float snow = upness > 0.6 ? 1 : 0;
    float snow = smoothstep(0.5, 0.6, upness);

    surf.Diffuse = (1 - snow) * surf.Diffuse + snow * vec3(1);
    surf.Normal = normalize((1 - snow) * surf.Normal + snow * vec3(0.0, 1.0, 0.0));

    GOutPositionRadiance = vec4(surf.WorldPosition, surf.Radiance);
    GOutNormalShininess = vec4(surf.Normal, surf.Shininess);
    GOutAlbedoSpecular = vec4(surf.Diffuse, surf.Specular);
}
