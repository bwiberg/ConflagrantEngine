#version 410

in mat3 fIn_WorldTBN;
in vec3 fIn_WorldPosition;
in vec2 fIn_TexCoord;

struct MaterialProperty {
    sampler2D map;
    vec3 color;
    int hasMap;
};

uniform struct {
    MaterialProperty diffuse;
    MaterialProperty specular;
    sampler2D normalMap;
    int hasNormalMap;
    float shininess;
} material;

struct PointLight {
    vec3 worldPosition;
    float intensity;
    vec3 color;
};

struct DirectionalLight {
    vec3 direction;
    float intensity;
    vec3 color;
};

#define MAX_NUM_LIGHTS 16
uniform PointLight pointLights[MAX_NUM_LIGHTS];
uniform int numPointLights = 0;
uniform DirectionalLight directionalLights[MAX_NUM_LIGHTS];
uniform int numDirectionalLights = 0;

uniform float AttenuationConstant = 1.0f;
uniform float AttenuationLinear = 0.1f;
uniform float AttenuationQuadratic = 0.01f;

uniform float time;
uniform vec3 EyePos;

out vec4 out_Color;

vec4 GetPropertyColor(MaterialProperty prop) {
    vec4 color = vec4(prop.color, 1);
    if (prop.hasMap != 0) {
        color = texture(prop.map, vec2(fIn_TexCoord.s, 1 - fIn_TexCoord.t));
    }

    return color;
}

float Attenuate(float d) {
    return 1.0 / (AttenuationConstant + AttenuationLinear * d + AttenuationQuadratic * d * d);
}

vec3 ComputePhongShading(vec3 L, vec3 N, vec3 E, vec3 lcolor, vec3 kDiffuse, vec3 kSpecular, float shininess) {
    float NL = max(dot(normalize(N), L), 0.0);
    vec3 diffuse = NL * lcolor * kDiffuse;

    vec3 R = normalize(reflect(-L, N));
    float ER = max(dot(E, R), 0.0);
    vec3 specular = pow(ER, shininess) * lcolor * kDiffuse * kSpecular;

    return diffuse + specular;
}

vec3 ApplyDirectionalLight(DirectionalLight l, vec3 N, vec3 E, vec3 kDiffuse, vec3 kSpecular, float shininess) {
    return ComputePhongShading(l.direction, N, E, l.intensity * l.color, kDiffuse, kSpecular, shininess);
}

vec3 ApplyPointLight(PointLight l, vec3 N, vec3 E, vec3 kDiffuse, vec3 kSpecular, float shininess) {
    vec3 L = l.worldPosition - fIn_WorldPosition;
    float distance = length(l.worldPosition - fIn_WorldPosition);
    L /= distance;

    return Attenuate(distance) * ComputePhongShading(L, N, E, l.intensity * l.color, kDiffuse, kSpecular, shininess);
}

void main(void) {
    vec3 result = vec3(0, 0, 0);

    vec3 N = fIn_WorldTBN[2];
    if (material.hasNormalMap != 0) {
        // N = 2.0 * texture(material.normalMap, fIn_TexCoord).rgb - vec3(1.0);
        // N = normalize(fIn_WorldTBN * N);
    }

    vec4 diffuse = GetPropertyColor(material.diffuse);
    float alpha = diffuse.a;
    if (alpha < 1.0 / 255) {
        discard;
    }
    vec3 kDiffuse = diffuse.rgb;

    vec3 kSpecular = GetPropertyColor(material.specular).rgb;
    float shininess = max(10, material.shininess);

    vec3 E = normalize(EyePos - fIn_WorldPosition);

    int i;
    for (i = 0; i < numPointLights; i++) {
        result += ApplyPointLight(pointLights[i], N, E, kDiffuse, kSpecular, shininess);
    }

    for (i = 0; i < numDirectionalLights; i++) {
        result += ApplyDirectionalLight(directionalLights[i], N, E, kDiffuse, kSpecular, shininess);
    }

    if (numPointLights == 0) {
        result += kDiffuse;
    }

    // result = N;
    out_Color =  vec4(result, alpha);
}
