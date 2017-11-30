#version 410

in mat3 fIn_WorldTBN;
in vec3 fIn_WorldPosition;
in vec2 fIn_TexCoord;

out vec4 out_Color;

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

#define MAX_NUM_LIGHTS 16
uniform PointLight pointLights[MAX_NUM_LIGHTS];
uniform int numPointLights = 0;

uniform float AttenuationConstant = 1.0f;
uniform float AttenuationLinear = 0.1f;
uniform float AttenuationQuadratic = 0.01f;

uniform float time;
uniform vec3 EyePos;

vec3 GetPropertyColor(MaterialProperty prop) {
    vec3 color = prop.color;
    if (prop.hasMap != 0) {
        color = texture(prop.map, fIn_TexCoord).rgb;
    }
    return color;
}

float Attenuate(float d) {
    return 1.0 / (AttenuationConstant + AttenuationLinear * d + AttenuationQuadratic * d * d);
}

vec3 ApplyPhongShading(PointLight l, vec3 Normal, vec3 kDiffuse, vec3 kSpecular, float shininess) {
    vec3 L = normalize(l.worldPosition - fIn_WorldPosition);

    float NL = max(dot(normalize(Normal), L), 0.0);
    vec3 diffuse = NL * l.intensity * l.color * kDiffuse;

    vec3 R = normalize(reflect(-L, Normal));

    vec3 E = normalize(EyePos - fIn_WorldPosition);
    float ER = max(dot(E, R), 0.0);
    vec3 specular = pow(ER, 1) * l.intensity * l.color * kDiffuse * kSpecular;

    float distance = length(l.worldPosition - fIn_WorldPosition);
    return Attenuate(distance) * (diffuse + specular);
}

void main(void) {
    vec3 result = vec3(0, 0, 0);

    vec3 Normal = vec3(0, 0, 1);
    if (material.hasNormalMap != 0) {
        Normal = 2.0 * texture(material.normalMap, fIn_TexCoord).rgb - vec3(1.0);
    }
    Normal = normalize(fIn_WorldTBN * Normal);

    vec3 kDiffuse = GetPropertyColor(material.diffuse);
    vec3 kSpecular = GetPropertyColor(material.specular);
    float shininess = material.shininess;

    int i;
    for (i = 0; i < numPointLights; i++) {
        result += ApplyPhongShading(pointLights[i], Normal, kDiffuse, kSpecular, shininess);
    }

    if (numPointLights == 0) {
        result += kDiffuse;
    }

    out_Color =  vec4(result, 1.0);
}
