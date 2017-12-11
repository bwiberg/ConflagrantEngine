#include "common/SurfaceInfo.glsl"

vec3 ComputePhongShading(SurfaceInfo surf, vec3 LightDir, vec3 LightColor, vec3 EyeDir) {
    float NL = max(dot(normalize(surf.Normal), LightDir), 0.0);
    vec3 diffuse = NL * LightColor * surf.Diffuse;

    vec3 R = normalize(reflect(-LightDir, surf.Normal));
    float ER = max(dot(EyeDir, R), 0.0);
    vec3 specular = pow(ER, surf.Shininess) * LightColor * surf.Diffuse * surf.Specular;

    return diffuse + specular;
}
