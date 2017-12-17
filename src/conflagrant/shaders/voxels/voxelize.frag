#version 450

#include "common/Definitions.glsl"
#include "common/DirectionalLight.glsl"
#include "common/Lighting.glsl"
#include "common/Material.glsl"
#include "common/PointLight.glsl"

in vec3 fIn_WorldPosition;
in vec3 fIn_VoxelUnitCubeCoord;
in mat3 fIn_WorldTBN;
in vec2 fIn_TexCoord;
in vec4 fIn_DirectionalLightSpacePositions[MAX_DIRECTIONALLIGHTS];

uniform PointLight pointLights[MAX_POINTLIGHTS];
uniform int numPointLights = 0;
uniform DirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
uniform int numDirectionalLights = 0;

//layout(RGBA8) uniform image3D VoxelizedScene;
uniform layout (r32ui) coherent volatile uimage3D VoxelizedScene;

uniform Material material;

uniform vec3 EyePos;
uniform vec3 VoxelHalfDimensions;
uniform vec3 VoxelCenter;

#include "voxels/common/util.glsl"

void main(void){
    // x, y and z go from -1 to 1
    vec3 voxelCoordinates = GetUnitCubeCoordinates(fIn_WorldPosition, VoxelCenter, VoxelHalfDimensions);
    if(!IsWithinVoxelColume(voxelCoordinates)) discard;

    // x, y and z go from 0 to 1, i.e. for indexing voxel volume
    voxelCoordinates = GetNormalizedCoordinatesFromUnitCubeCoordinates(voxelCoordinates);

    vec3 result = vec3(0, 0, 0);

    vec3 N = fIn_WorldTBN[2];
    if (material.hasNormalMap != 0) {
        N = 2.0 * texture(material.normalMap, fIn_TexCoord).rgb - vec3(1.0);
        N = normalize(fIn_WorldTBN * N);
    }

    vec4 diffuse = GetPropertyColor(material.diffuse, fIn_TexCoord);
    float alpha = diffuse.a;
    if (alpha < 1.0 / 255) {
        discard;
    }

    SurfaceInfo surf;
    surf.WorldPosition = fIn_WorldPosition;
    surf.Normal = N;
    surf.Diffuse = diffuse.rgb;
    surf.Specular = 0; // max(max(specular.r, specular.g), specular.b);
    surf.Shininess = max(1, material.shininess);

    vec3 E = normalize(EyePos - fIn_WorldPosition);

    int i;
    for (i = 0; i < numPointLights; i++) {
        result += ApplyPointLight(surf, pointLights[i], E);
    }

    for (i = 0; i < numDirectionalLights; i++) {
        result += ApplyDirectionalLight(surf, directionalLights[i],
            fIn_DirectionalLightSpacePositions[i], E);
    }

    //result += 0.1 * diffuse.rgb;

    if (numPointLights == 0 && numDirectionalLights == 0) {
        result = diffuse.rgb;
    }

    ivec3 imageCoords = GetIntegerCoordinatesFromNormalizedTextureCoordinates(imageSize(VoxelizedScene), voxelCoordinates);
    ImageAtomicAverageRGBA8(VoxelizedScene, imageCoords, result);
    // imageStore(VoxelizedScene, imageCoords, vec4(result, alpha));
}
