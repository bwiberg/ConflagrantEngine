#version 450 core

#include "common/Blending.glsl"
#include "common/ComponentWise.glsl"
#include "common/Constants.glsl"
#include "common/Definitions.glsl"
#include "common/DirectionalLight.glsl"
#include "common/Lighting.glsl"
#include "common/PointLight.glsl"
#include "common/GeometryFunctions.glsl"
#include "voxels/common/util.glsl"

in vec2 fIn_TexCoord;

uniform PointLight pointLights[MAX_POINTLIGHTS];
uniform int numPointLights = 0;
uniform DirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
uniform int numDirectionalLights = 0;

uniform sampler2D GPositionRadiance;
uniform sampler2D GNormalShininess;
uniform sampler2D GAlbedoSpecular;

uniform float time;
uniform vec3 EyePos;

uniform sampler3D VoxelizedScene;
uniform vec3 VoxelHalfDimensions;
uniform vec3 VoxelCenter;
uniform float VoxelSize;

uniform float DirectMultiplier = 1.0;
uniform float IndirectDiffuseMultiplier = 1.0;
uniform float IndirectSpecularMultiplier = 1.0;

out vec4 out_Color;

vec3 TraceVoxelCone(const vec3 Origin, const vec3 Direction, const float MipmapFactor, const float ColorBoost) {
    vec3 color = vec3(0);
    float alpha = 0;

    float t = VCT_INDIRECT_START_BIAS;
    while(alpha < 1){
        vec3 worldPosition = Origin + t * Direction;

        const float MipmapLevel = MipmapFactor * log2((1 + VCT_INDIRECT_SPREAD * t / VoxelSize));
        const float SamplePower = (MipmapLevel + 1) * (MipmapLevel + 1);

        vec3 voxelCoordinates = GetUnitCubeCoordinates(worldPosition, VoxelCenter, VoxelHalfDimensions);
        if (!IsWithinVoxelColume(voxelCoordinates)) {
            break;
        }
        voxelCoordinates = GetNormalizedCoordinatesFromUnitCubeCoordinates(voxelCoordinates);
        vec4 voxel = textureLod(VoxelizedScene, voxelCoordinates, min(VCT_MIPMAP_MAX, MipmapLevel));
        voxel.rgb *= 1 + ColorBoost;

        if (MipmapLevel == 0) {
            voxel.a = voxel.a > 0 ? 1 : 0;
        }

        AlphaBlend_FrontToBack(color, alpha, voxel.rgb, voxel.a);

        t += CONETRACING_STEP_FACTOR * SamplePower * VoxelSize * VoxelHalfDimensions.x;
    }

    return color * alpha;
}

vec3 ApplyIndirectSpecularLight(SurfaceInfo surf, vec3 EyeDir, float Diffusion) {
    const vec3 Origin = surf.WorldPosition + surf.Normal * (1 + 4 * ISQRT2) * VoxelSize;
    const vec3 Direction = normalize(reflect(-EyeDir, surf.Normal));

    return surf.Diffuse * TraceVoxelCone(Origin, Direction, Diffusion, Diffusion);
}

vec3 ApplyIndirectDiffuseLight(SurfaceInfo surf) {
    vec3 result = vec3(0);

    vec3 Tangent, Bitangent;
    MakeOrthonormalBasisTBN(surf.Normal, Tangent, Bitangent);

    const vec3 Origin = surf.WorldPosition + surf.Normal * (1 + 4 * ISQRT2) * VoxelSize;

    const vec3 TPos = normalize(mix(surf.Normal,  Tangent,   VCT_INDIRECT_ORTHOGONALITY));
    const vec3 TNeg = normalize(mix(surf.Normal, -Tangent,   VCT_INDIRECT_ORTHOGONALITY));
    const vec3 BPos = normalize(mix(surf.Normal,  Bitangent, VCT_INDIRECT_ORTHOGONALITY));
    const vec3 BNeg = normalize(mix(surf.Normal, -Bitangent, VCT_INDIRECT_ORTHOGONALITY));

    result += VCT_DIFFUSE_NORMAL_WEIGHT * TraceVoxelCone(Origin + VCT_INDIRECT_OFFSET * surf.Normal, surf.Normal, 1, 0);
    result += VCT_DIFFUSE_SIDE_WEIGHT   * TraceVoxelCone(Origin + VCT_INDIRECT_OFFSET * Tangent,     TPos, 1, 0);
    result += VCT_DIFFUSE_SIDE_WEIGHT   * TraceVoxelCone(Origin - VCT_INDIRECT_OFFSET * Tangent,     TNeg, 1, 0);
    result += VCT_DIFFUSE_SIDE_WEIGHT   * TraceVoxelCone(Origin + VCT_INDIRECT_OFFSET * Bitangent,   BPos, 1, 0);
    result += VCT_DIFFUSE_SIDE_WEIGHT   * TraceVoxelCone(Origin - VCT_INDIRECT_OFFSET * Bitangent,   BNeg, 1, 0);

    return surf.Diffuse * result;
}

void TextureVec3AndFloat(sampler2D sampler, vec2 texCoords, out vec3 outvec3, out float outfloat) {
    vec4 value = texture(sampler, texCoords);
    outvec3 = value.xyz;
    outfloat = value.w;
}

void main(void) {
    vec3 result = vec3(0, 0, 0);

    SurfaceInfo surf;
    TextureVec3AndFloat(GPositionRadiance, fIn_TexCoord, surf.WorldPosition, surf.Radiance);
    surf.Radiance *= VCT_RADIANCE_MAX;
    TextureVec3AndFloat(GNormalShininess, fIn_TexCoord, surf.Normal, surf.Shininess);
    TextureVec3AndFloat(GAlbedoSpecular, fIn_TexCoord, surf.Diffuse, surf.Specular);

    if (length(surf.Normal) < 0.1) {
        // invalid normal => no fragment here
        discard;
    }

    vec3 E = normalize(EyePos - surf.WorldPosition);

    // calculate direct lighting
    int i;
    for (i = 0; i < numPointLights; i++) {
        result += ApplyPointLight(surf, pointLights[i], E);
    }

    for (i = 0; i < numDirectionalLights; i++) {
        result += ApplyDirectionalLight(surf, directionalLights[i],
                                        directionalLights[i].VP * vec4(surf.WorldPosition, 1), E);
    }
    result *= DirectMultiplier;

    // emit light from radiant surfaces
    result += surf.Diffuse * surf.Radiance;

    // calculate indirect lighting
    result += IndirectDiffuseMultiplier
            * VCT_DIFFUSE_STRENGTH * ApplyIndirectDiffuseLight(surf);
    result += IndirectSpecularMultiplier
            * surf.Specular * VCT_SPECULAR_STRENGTH * ApplyIndirectSpecularLight(surf, E,
                                                        VCT_DIFFUSION_MULTIPLIER * surf.Specular);

    if (numPointLights == 0 && numDirectionalLights == 0) {
        result = surf.Diffuse;
    }

    //result = vec3(surf.Specular);

    out_Color = vec4(pow(result, vec3(GAMMA)), 1);
}
