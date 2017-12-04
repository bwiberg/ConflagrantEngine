#version 410

in float fIn_ModelY;
in vec2 fIn_TexCoord;

uniform sampler2D skydomeColor;

uniform float time;
uniform vec3 EyePos;

out vec4 out_Color;

void main(void) {
    vec3 sky = textureLod(skydomeColor, fIn_TexCoord, 0).rgb;
    vec3 ground = vec3(0.3, 0.3, 0.3);

    float ksky = step(0, fIn_ModelY);
    vec3 result = ksky * sky + (1 - ksky) * ground;
    out_Color =  vec4(result, 1.0);
}
