uniform float AttenuationConstant = 1.0f;
uniform float AttenuationLinear = 0.1f;
uniform float AttenuationQuadratic = 0.01f;

float Attenuate(float d) {
    return 1.0 / (AttenuationConstant + AttenuationLinear * d + AttenuationQuadratic * d * d);
}
