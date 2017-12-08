////////////////////////////////////
// Modulo 289 without a division  //
////////////////////////////////////

float mod289(float x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}


/////////////////////////////////
// Modulo 7 without a division //
/////////////////////////////////

vec3 mod7(vec3 x) {
    return x - floor(x * (1.0 / 7.0)) * 7.0;
}

vec4 mod7(vec4 x) {
    return x - floor(x * (1.0 / 7.0)) * 7.0;
}

/////////////////////////////////////////////////
// Permutation polynomial: (34x^2 + x) mod 289 //
/////////////////////////////////////////////////

float permute(float x) {
    return mod289(((x*34.0)+1.0)*x);
}

vec2 permute(vec2 x) {
    return mod289((34.0 * x + 1.0) * x);
}

vec3 permute(vec3 x) {
    return mod289((34.0 * x + 1.0) * x);
}

vec4 permute(vec4 x) {
    return mod289((34.0 * x + 1.0) * x);
}


///////////////////////////////////////////////////////
// Fast inverse square root (using Taylor expansion) //
///////////////////////////////////////////////////////

float taylorInvSqrt(float r) {
    return 1.79284291400159 - 0.85373472095314 * r;
}

vec4 taylorInvSqrt(vec4 r) {
    return 1.79284291400159 - 0.85373472095314 * r;
}

vec2 fade(vec2 t) {
    return t*t*t*(t*(t*6.0-15.0)+10.0);
}

vec3 fade(vec3 t) {
    return t*t*t*(t*(t*6.0-15.0)+10.0);
}

vec4 fade(vec4 t) {
    return t*t*t*(t*(t*6.0-15.0)+10.0);
}
