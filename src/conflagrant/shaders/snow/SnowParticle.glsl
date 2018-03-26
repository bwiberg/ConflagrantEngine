struct Particle {
    vec3 position;
    vec3 velocity;
    float angle;
    float lifetime;
};

#ifdef USE_IMAGE
#include "common/UnpackImage.glsl"

Particle LoadParticle(int index) {
    Particle p;

    ivec2 coords = ivec2(index, 0);
    ImageLoad2D_vec3_float(PositionsAngles,     coords, p.position, p.angle);
    ImageLoad2D_vec3_float(VelocitiesLifetimes, coords, p.velocity, p.lifetime);

    return p;
}

void StoreParticle(int index, Particle p) {
    ivec2 coords = ivec2(index, 0);
    ImageStore2D_vec3_float(PositionsAngles,     coords, p.position, p.angle);
    ImageStore2D_vec3_float(VelocitiesLifetimes, coords, p.velocity, p.lifetime);
}

#endif

#ifdef USE_TEXTURE
#include "common/UnpackTexture.glsl"

Particle LoadParticle(vec2 texCoord) {
    Particle p;

    Texture_vec3_float(InPositionsAngles,     texCoord, p.position, p.angle);
    Texture_vec3_float(InVelocitiesLifetimes, texCoord, p.velocity, p.lifetime);

    return p;
}

void StoreParticle(Particle p) {
    OutPositionAngle = vec4(p.position, p.angle);
    OutVelocityLifetime = vec4(p.velocity, p.lifetime);
}

#endif

