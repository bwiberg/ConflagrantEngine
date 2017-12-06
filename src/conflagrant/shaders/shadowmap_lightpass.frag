#version 410

in vec3 fIn_WorldPosition;
in vec2 fIn_TexCoord;

struct MaterialProperty {
    sampler2D map;
    vec3 color;
    int hasMap;
};

uniform struct {
    MaterialProperty diffuse;
} material;

uniform float time;

vec4 GetPropertyColor(MaterialProperty prop) {
    vec4 color = vec4(prop.color, 1);
    if (prop.hasMap != 0) {
        vec2 st = vec2(fIn_TexCoord.s, 1 - fIn_TexCoord.t);
        float mipmapLevel = textureQueryLod(prop.map, st).x;
        color = textureLod(prop.map, st, mipmapLevel);
    }

    return color;
}

void main(void) {
    float alpha = GetPropertyColor(material.diffuse).a;
    if (alpha < 1.0 / 255) {
        discard;
    }
}
