#include "common/SurfaceInfo.glsl"
#include "common/Lighting.glsl"
#include "common/Attenuation.glsl"

struct PointLight {
    vec3 worldPosition;
    float intensity;
    vec3 color;
};

vec3 ApplyPointLight(SurfaceInfo surf, PointLight l, vec3 EyeDir) {
    vec3 L = l.worldPosition - surf.WorldPosition;
    float distance = length(L);
    L /= distance;

    return Attenuate(distance) * ComputePhongShading(surf, L, l.intensity * l.color, EyeDir);
}
