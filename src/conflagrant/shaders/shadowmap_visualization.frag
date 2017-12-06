#version 410

in vec2 fIn_TexCoord;

uniform sampler2D shadowMap;

out vec4 out_Color;

void main(void) {
    float depth = texture(shadowMap, fIn_TexCoord).r;
    // depth = 1;
    out_Color = vec4(depth, depth, depth, 1);
}
