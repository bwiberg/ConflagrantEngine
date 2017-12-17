#version 450 core

#include "common/Blending.glsl"
#include "common/ComponentWise.glsl"
#include "common/Definitions.glsl"
#include "voxels/common/util.glsl"

in vec3 fIn_RayOrigin;
in vec3 fIn_RayDirection;

uniform sampler3D VoxelizedScene;
uniform vec3 EyePos;
uniform vec3 VoxelHalfDimensions;
uniform vec3 VoxelCenter;
uniform int MipmapLevel;
uniform int NumSteps = RAYMARCH_MAX_STEPS;
uniform float BreakOnAlpha = 0.99;
uniform float RenderDistance;

out vec4 out_Color;

#include "common/ease/quadratic-in.glsl"
#define EASE(t) quadraticIn(t)

void main(void) {
	const int N = min(NumSteps, RAYMARCH_MAX_STEPS);

    const float fraction = 1.0 / (N - 1);
    vec3 color = vec3(0);
    float alpha = 0;

	for(int i = 0; i < N && alpha < BreakOnAlpha; ++i) {
	    vec3 worldPosition = fIn_RayOrigin + EASE(i * fraction) * RenderDistance * fIn_RayDirection;

        vec3 voxelCoordinates = GetUnitCubeCoordinates(worldPosition, VoxelCenter, VoxelHalfDimensions);
        if(!IsWithinVoxelColume(voxelCoordinates)) continue;

        voxelCoordinates = GetNormalizedCoordinatesFromUnitCubeCoordinates(voxelCoordinates);
		vec4 texel = textureLod(VoxelizedScene, voxelCoordinates, MipmapLevel);
		texel.a = MipmapLevel > 0 ? texel.a : (Max(texel.rgb) > 0 ? 1 : 0);

        AlphaBlend_FrontToBack(color, alpha, texel.rgb, texel.a);
	}

	out_Color = vec4(color, alpha);
}
