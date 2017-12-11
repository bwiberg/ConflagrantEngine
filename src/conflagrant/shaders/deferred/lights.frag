#version 410

#include "common/Definitions.glsl"
#include "common/DirectionalLight.glsl"
#include "common/Lighting.glsl"
#include "common/PointLight.glsl"

in vec2 fIn_TexCoord;

uniform PointLight pointLights[MAX_POINTLIGHTS];
uniform int numPointLights = 0;
uniform DirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
uniform int numDirectionalLights = 0;

uniform sampler2D GPosition;
uniform sampler2D GNormalShininess;
uniform sampler2D GAlbedoSpecular;

uniform float time;
uniform vec3 EyePos;

out vec4 out_Color;

void TextureVec3AndFloat(sampler2D sampler, vec2 texCoords, out vec3 outvec3, out float outfloat) {
    vec4 value = texture(sampler, texCoords);
    outvec3 = value.xyz;
    outfloat = value.w;
}

void main(void) {
    vec3 result = vec3(0, 0, 0);

    SurfaceInfo surf;
    surf.WorldPosition = texture(GPosition, fIn_TexCoord).xyz;
    TextureVec3AndFloat(GNormalShininess, fIn_TexCoord, surf.Normal, surf.Shininess);
    TextureVec3AndFloat(GAlbedoSpecular, fIn_TexCoord, surf.Diffuse, surf.Specular);

    vec3 E = normalize(EyePos - surf.WorldPosition);

    int i;
    for (i = 0; i < numPointLights; i++) {
        result += ApplyPointLight(surf, pointLights[i], E);
    }

    for (i = 0; i < numDirectionalLights; i++) {
        result += ApplyDirectionalLight(surf, directionalLights[i],
                                        directionalLights[i].VP * vec4(surf.WorldPosition, 1), E);
    }

    result += 0.1 * surf.Diffuse;

    if (numPointLights == 0 && numDirectionalLights == 0) {
        result = surf.Diffuse;
    }

    out_Color = vec4(result, 1);
}
