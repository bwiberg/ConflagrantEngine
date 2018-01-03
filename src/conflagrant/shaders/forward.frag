#version 410

#include "common/Definitions.glsl"
#include "common/DirectionalLight.glsl"
#include "common/Lighting.glsl"
#include "common/Material.glsl"
#include "common/PointLight.glsl"

in mat3 fIn_WorldTBN;
in vec3 fIn_WorldPosition;
in vec2 fIn_TexCoord;
in vec4 fIn_DirectionalLightSpacePositions[MAX_DIRECTIONALLIGHTS];

uniform PointLight pointLights[MAX_POINTLIGHTS];
uniform int numPointLights = 0;
uniform DirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
uniform int numDirectionalLights = 0;

uniform Material material;

uniform float time;
uniform vec3 EyePos;

out vec4 out_Color;


void main(void) {
    vec3 result = vec3(0, 0, 0);

    vec3 N = fIn_WorldTBN[2];
    if (material.hasNormalMap != 0) {
        N = 2.0 * texture(material.normalMap, 1 - fIn_TexCoord).rgb - vec3(1.0);
        N = normalize(fIn_WorldTBN * N);
    }

    vec4 diffuse = GetPropertyColor(material.diffuse, fIn_TexCoord);
    float alpha = diffuse.a;
    if (alpha < 1.0 / 255) {
        discard;
    }

    vec3 specular = GetPropertyColor(material.specular, fIn_TexCoord).rgb;

    SurfaceInfo surf;
    surf.WorldPosition = fIn_WorldPosition;
    surf.Normal = N;
    surf.Diffuse = diffuse.rgb;
    surf.Specular = max(max(specular.r, specular.g), specular.b);
    surf.Shininess = max(1, material.shininess);

    vec3 E = normalize(EyePos - fIn_WorldPosition);

    int i;
    for (i = 0; i < numPointLights; i++) {
        result += ApplyPointLight(surf, pointLights[i], E);
    }

    for (i = 0; i < numDirectionalLights; i++) {
        result += ApplyDirectionalLight(surf, directionalLights[i],
            fIn_DirectionalLightSpacePositions[i], E);
    }

    result += 0.1 * diffuse.rgb;

    if (numPointLights == 0 && numDirectionalLights == 0) {
        result = diffuse.rgb;
    }

    out_Color =  vec4(result, alpha);
}
