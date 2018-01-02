struct MaterialProperty {
    sampler2D map;
    vec3 color;
    int hasMap;
};

struct Material {
    MaterialProperty diffuse;
    MaterialProperty specular;
    sampler2D normalMap;
    int hasNormalMap;
    float shininess;
    float radiance;
};

vec4 GetPropertyColor(MaterialProperty prop, vec2 texCoord) {
    vec4 color = vec4(prop.color, 1);
    if (prop.hasMap != 0) {
        vec2 st = vec2(texCoord.s, 1 - texCoord.t);
        float mipmapLevel = textureQueryLod(prop.map, st).x;
        color = textureLod(prop.map, st, mipmapLevel);
    }

    return color;
}
