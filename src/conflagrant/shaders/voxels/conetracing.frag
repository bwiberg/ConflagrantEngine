#version 450 core

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

uniform sampler2D GPosition;
uniform sampler2D GNormalShininess;
uniform sampler2D GAlbedoSpecular;

uniform float time;
uniform vec3 EyePos;

uniform sampler3D VoxelizedScene;
uniform vec3 VoxelHalfDimensions;
uniform vec3 VoxelCenter;
uniform float VoxelSize;

out vec4 out_Color;

vec3 TraceDiffuse(const vec3 Origin, const vec3 Direction) {
    vec4 result = vec4(0.0f);

    float t = VCT_INDIRECT_START_BIAS;
    while(result.a < 1){
        vec3 worldPosition = Origin + t * Direction;

        const float MipmapLevel = log2((1 + VCT_INDIRECT_SPREAD * t / VoxelSize));
        const float SamplePower = (MipmapLevel + 1) * (MipmapLevel + 1);

        vec3 voxelCoordinates = GetUnitCubeCoordinates(worldPosition, VoxelCenter, VoxelHalfDimensions);
        if (!IsWithinVoxelColume(voxelCoordinates)) {
            break;
        }
        voxelCoordinates = GetNormalizedCoordinatesFromUnitCubeCoordinates(voxelCoordinates);
        vec4 voxel = textureLod(VoxelizedScene, voxelCoordinates, min(VCT_MIPMAP_MAX, MipmapLevel));
        voxel.a = max(voxel.r, max(voxel.g, voxel.b));
        result += 0.075 * SamplePower * voxel * pow(1 - voxel.a, 2);
        result.a += 0.1;

        t += SamplePower * VoxelSize;
    }

    return result.rgb * result.a;
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

    result += VCT_INDIRECT_NORMAL_WEIGHT * TraceDiffuse(Origin + VCT_INDIRECT_OFFSET * surf.Normal, surf.Normal);
    result += VCT_INDIRECT_SIDE_WEIGHT   * TraceDiffuse(Origin + VCT_INDIRECT_OFFSET * Tangent,     TPos);
    result += VCT_INDIRECT_SIDE_WEIGHT   * TraceDiffuse(Origin - VCT_INDIRECT_OFFSET * Tangent,     TNeg);
    result += VCT_INDIRECT_SIDE_WEIGHT   * TraceDiffuse(Origin + VCT_INDIRECT_OFFSET * Bitangent,   BPos);
    result += VCT_INDIRECT_SIDE_WEIGHT   * TraceDiffuse(Origin - VCT_INDIRECT_OFFSET * Bitangent,   BNeg);

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
    surf.WorldPosition = texture(GPosition, fIn_TexCoord).xyz;
    TextureVec3AndFloat(GNormalShininess, fIn_TexCoord, surf.Normal, surf.Shininess);
    TextureVec3AndFloat(GAlbedoSpecular, fIn_TexCoord, surf.Diffuse, surf.Specular);

    vec3 E = normalize(EyePos - surf.WorldPosition);

    int i;
    for (i = 0; i < numPointLights; i++) {
        result += ApplyPointLight(surf, pointLights[i], E);
    }

    for (i = 0; i < numDirectionalLights; i++) {
        result += ApplyDirectionalLight(surf, directionalLights[i],
                                        directionalLights[i].VP * vec4(surf.WorldPosition, 1), E);
    }

    result += VCT_INDIRECT_STRENGTH * ApplyIndirectDiffuseLight(surf);

    if (numPointLights == 0 && numDirectionalLights == 0) {
        result = surf.Diffuse;
    }

    out_Color = vec4(pow(result, vec3(GAMMA)), 1);
}
