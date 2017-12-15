#version 450 core

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
    vec4 result = vec4(0);

    const float fraction = 1.0 / (N - 1);
    float any = 0.0;
	for(int i = 0; i < N && result.a < BreakOnAlpha; ++i) {
	    vec3 worldPosition = fIn_RayOrigin + EASE(i * fraction) * RenderDistance * fIn_RayDirection;

        vec3 voxelCoordinates = GetUnitCubeCoordinates(worldPosition, VoxelCenter, VoxelHalfDimensions);
        if(!IsWithinVoxelColume(voxelCoordinates)) continue;

        voxelCoordinates = GetNormalizedCoordinatesFromUnitCubeCoordinates(voxelCoordinates);
		vec4 voxelColor = textureLod(VoxelizedScene, voxelCoordinates, MipmapLevel);

		result += (1.0 - result.a) * voxelColor;
	}

	// result = vec4(any, any, any, 1);
	out_Color = result;
}
