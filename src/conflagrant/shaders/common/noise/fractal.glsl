#ifdef FRACTAL_NOISE_FUNCTION2
float fractal_noise(vec2 coords, int octaves, float scale, float amplitude, float lacunarity, float persistence) {
    coords *= scale;

    float value = 0;
    for (int i = 0; i < octaves; ++i) {
        value += amplitude * FRACTAL_NOISE_FUNCTION2(coords);
        coords *= lacunarity;
        amplitude *= persistence;
    }

    return value;
}
#endif

#ifdef FRACTAL_NOISE_FUNCTION3
float fractal_noise(vec3 coords, int octaves, float scale, float amplitude, float lacunarity, float persistence) {
    coords *= scale;

    float value = 0;
    for (int i = 0; i < octaves; ++i) {
        value += amplitude * FRACTAL_NOISE_FUNCTION3(coords);
        coords *= lacunarity;
        amplitude *= persistence;
    }

    return value;
}
#endif

#ifdef FRACTAL_NOISE_FUNCTION4
float fractal_noise(vec4 coords, int octaves, float scale, float amplitude, float lacunarity, float persistence) {
    coords *= scale;

    float value = 0;
    for (int i = 0; i < octaves; ++i) {
        value += amplitude * FRACTAL_NOISE_FUNCTION4(coords);
        coords *= lacunarity;
        amplitude *= persistence;
    }

    return value;
}
#endif
