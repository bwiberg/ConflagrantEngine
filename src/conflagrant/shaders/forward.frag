#version 410

#define MAX_POINTLIGHTS 16
#define MAX_DIRECTIONALLIGHTS 16

#define SINGLE_SAMPLE 0
#define PCF 1
    #define PCF_RADIUS 1
#define POISSON 2
#define STRATIFIED_POISSON 3
    #define POISSON_SAMPLES 16
    #define POISSON_VALUE 700.0

#define SHADOWMAP_METHOD STRATIFIED_POISSON

#define SHADOWMAP_BIAS 0.005
#define USE_SAMPLER2DSHADOW 1

#if USE_SAMPLER2DSHADOW == 1
    #define SAMPLER2DSHADOW sampler2DShadow
    #define SAMPLE_SHADOWMAP(shadowMap, projCoords, bias) texture(shadowMap, projCoords, bias)
#else // USE_SAMPLER2DSHADOW != 1
    #define SAMPLER2DSHADOW sampler2D
    #define SAMPLE_SHADOWMAP(shadowMap, projCoords, bias) texture(l.shadowMap, projCoords.xy).r
#endif // USE_SAMPLER2DSHADOW


in mat3 fIn_WorldTBN;
in vec3 fIn_WorldPosition;
in vec2 fIn_TexCoord;
in vec4 fIn_DirectionalLightSpacePositions[MAX_DIRECTIONALLIGHTS];

struct PointLight {
    vec3 worldPosition;
    float intensity;
    vec3 color;
};

struct DirectionalLight {
    vec3 direction;
    float intensity;
    vec3 color;

    int hasShadowMap;
#if USE_SAMPLER2DSHADOW == 1
    sampler2DShadow shadowMap;
#else
    sampler2D shadowMap;
#endif
    mat4 VP;
};

uniform PointLight pointLights[MAX_POINTLIGHTS];
uniform int numPointLights = 0;
uniform DirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
uniform int numDirectionalLights = 0;

uniform float AttenuationConstant = 1.0f;
uniform float AttenuationLinear = 0.1f;
uniform float AttenuationQuadratic = 0.01f;

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

uniform float time;
uniform vec3 EyePos;

out vec4 out_Color;

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

vec4 GetPropertyColor(MaterialProperty prop) {
    vec4 color = vec4(prop.color, 1);
    if (prop.hasMap != 0) {
        vec2 st = vec2(fIn_TexCoord.s, 1 - fIn_TexCoord.t);
        float mipmapLevel = textureQueryLod(prop.map, st).x;
        color = textureLod(prop.map, st, mipmapLevel);
    }

    return color;
}

float Attenuate(float d) {
    return 1.0 / (AttenuationConstant + AttenuationLinear * d + AttenuationQuadratic * d * d);
}

float ComputeVisibilityPCF(SAMPLER2DSHADOW shadowMap, vec3 projCoords, float bias) {
    float shadow = 0.0;
    vec3 pcfMult = vec3(1.0 / textureSize(shadowMap, 0), 0);
    for (int x = -PCF_RADIUS; x <= PCF_RADIUS; ++x) {
        for (int y = -PCF_RADIUS; y <= PCF_RADIUS; ++y) {
            float closestDepth = SAMPLE_SHADOWMAP(shadowMap, (projCoords + vec3(x, y, 0) * pcfMult), bias);
            shadow += projCoords.z - SHADOWMAP_BIAS < closestDepth ? 1.0 : 0.0;
        }
    }
    int samples = 2 * PCF_RADIUS + 1;
    return (shadow / (samples * samples));
}

float ComputeVisibilityPoisson(SAMPLER2DSHADOW shadowMap, vec3 projCoords, float bias, vec3 seed) {
    const vec2 poissonDisk[16] = vec2[](
       vec2( -0.94201624, -0.39906216 ),
       vec2( 0.94558609, -0.76890725 ),
       vec2( -0.094184101, -0.92938870 ),
       vec2( 0.34495938, 0.29387760 ),
       vec2( -0.91588581, 0.45771432 ),
       vec2( -0.81544232, -0.87912464 ),
       vec2( -0.38277543, 0.27676845 ),
       vec2( 0.97484398, 0.75648379 ),
       vec2( 0.44323325, -0.97511554 ),
       vec2( 0.53742981, -0.47373420 ),
       vec2( -0.26496911, -0.41893023 ),
       vec2( 0.79197514, 0.19090188 ),
       vec2( -0.24188840, 0.99706507 ),
       vec2( -0.81409955, 0.91437590 ),
       vec2( 0.19984126, 0.78641367 ),
       vec2( 0.14383161, -0.14100790 )
    );

    float shadow = 0.0;
    for (int i = 0; i < POISSON_SAMPLES; ++i) {
        int index = int(16.0 * random(seed, i)) % 16;
        float closestDepth = SAMPLE_SHADOWMAP(shadowMap, (projCoords + vec3(poissonDisk[index], 0) / POISSON_VALUE), bias);
        shadow += projCoords.z - SHADOWMAP_BIAS < closestDepth ? 1.0 : 0.0;
    }

    return shadow / POISSON_SAMPLES;
}

float ComputeVisibility(DirectionalLight l, vec4 lightspacePosition, vec3 N) {
    float visibility;
    if (l.hasShadowMap == 0) {
        visibility = 1.0;
    } else {
        vec3 projCoords = lightspacePosition.xyz / lightspacePosition.w;
        projCoords = 0.5 * projCoords + 0.5;

        float bias = max(10 * SHADOWMAP_BIAS * (1.0 - dot(N, l.direction)), SHADOWMAP_BIAS);

        float closestDepth = SAMPLE_SHADOWMAP(l.shadowMap, projCoords, bias);

#if SHADOWMAP_METHOD == SINGLE_SAMPLE
        visibility = projCoords.z - SHADOWMAP_BIAS < closestDepth ? 1.0 : 0.0;
#elif SHADOWMAP_METHOD == PCF
        visibility = ComputeVisibilityPCF(l.shadowMap, projCoords, bias);
#elif SHADOWMAP_METHOD == POISSON
        visibility = ComputeVisibilityPoisson(l.shadowMap, projCoords, bias, vec3(0, 0, 0));
#elif SHADOWMAP_METHOD == STRATIFIED_POISSON
        visibility = ComputeVisibilityPoisson(l.shadowMap, projCoords, bias, fIn_WorldPosition);
#else
        visibility = 0.0;
#endif

        if (projCoords.z > 1.0) {
            visibility = 1.0;
        }
    }
    return visibility;
}

vec3 ComputePhongShading(vec3 L, vec3 N, vec3 E, vec3 lcolor, vec3 kDiffuse, vec3 kSpecular, float shininess) {
    float NL = max(dot(normalize(N), L), 0.0);
    vec3 diffuse = NL * lcolor * kDiffuse;

    vec3 R = normalize(reflect(-L, N));
    float ER = max(dot(E, R), 0.0);
    vec3 specular = pow(ER, shininess) * lcolor * kDiffuse * kSpecular;

    return diffuse + specular;
}

vec3 ApplyDirectionalLight(DirectionalLight l, vec4 lightspacePosition, vec3 N, vec3 E, vec3 kDiffuse, vec3 kSpecular, float shininess) {
    return ComputeVisibility(l, lightspacePosition, N)
        * ComputePhongShading(l.direction, N, E, l.intensity * l.color, kDiffuse, kSpecular, shininess);
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
        N = 2.0 * texture(material.normalMap, fIn_TexCoord).rgb - vec3(1.0);
        N = normalize(fIn_WorldTBN * N);
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
        result += ApplyPointLight(pointLights[i],
            N, E, kDiffuse, kSpecular, shininess);
    }

    for (i = 0; i < numDirectionalLights; i++) {
        result += ApplyDirectionalLight(directionalLights[i],
            fIn_DirectionalLightSpacePositions[i], N, E, kDiffuse, kSpecular, shininess);
    }

    if (numPointLights == 0 && numDirectionalLights == 0) {
        result += kDiffuse;
    }

    float v = ComputeVisibility(directionalLights[0], fIn_DirectionalLightSpacePositions[0], N);
    //result = vec3(v, v, v);
    vec4 k = fIn_DirectionalLightSpacePositions[0];
    //result = abs(k.xyz / k.w);
    out_Color =  vec4(result, alpha);
}
