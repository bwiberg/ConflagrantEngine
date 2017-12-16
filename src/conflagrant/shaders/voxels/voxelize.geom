#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

#include "common/Definitions.glsl"

in vec3 gIn_WorldPosition[3];
in mat3 gIn_WorldTBN[3];
in vec2 gIn_TexCoord[3];
in vec4 gIn_DirectionalLightSpacePositions[3][MAX_DIRECTIONALLIGHTS];

out vec3 fIn_WorldPosition;
out vec3 fIn_VoxelUnitCubeCoord;
out mat3 fIn_WorldTBN;
out vec2 fIn_TexCoord;
out vec4 fIn_DirectionalLightSpacePositions[MAX_DIRECTIONALLIGHTS];

uniform vec3 VoxelHalfDimensions;
uniform vec3 VoxelCenter;

#include "voxels/common/util.glsl"

void main(void) {
    const vec3 p = abs(cross(gIn_WorldPosition[1] - gIn_WorldPosition[0],
                             gIn_WorldPosition[2] - gIn_WorldPosition[0]));

    mat3 AxisMatrix = mat3(0);

    // Z-axis dominant
    if(p.z > p.x && p.z > p.y) {
        // gl_Position = vec4(fIn_VoxelUnitCubeCoord.x, fIn_VoxelUnitCubeCoord.y, 0, 1);
        AxisMatrix[0][0] = 1;
        AxisMatrix[1][1] = 1;
    }

    // X-axis dominant
    else if (p.x > p.y && p.x > p.z) {
        // gl_Position = vec4(fIn_VoxelUnitCubeCoord.y, fIn_VoxelUnitCubeCoord.z, 0, 1);
        AxisMatrix[1][0] = 1;
        AxisMatrix[2][1] = 1;
    }

    // Y-axis dominant
    else {
        // gl_Position = vec4(fIn_VoxelUnitCubeCoord.x, fIn_VoxelUnitCubeCoord.z, 0, 1);
        AxisMatrix[0][0] = 1;
        AxisMatrix[2][1] = 1;
    }

    for (int i = 0; i < 3; i++) {
        fIn_WorldPosition = gIn_WorldPosition[i];
        fIn_VoxelUnitCubeCoord = GetUnitCubeCoordinates(fIn_WorldPosition, VoxelCenter, VoxelHalfDimensions);
        fIn_WorldTBN = gIn_WorldTBN[i];
        fIn_TexCoord = gIn_TexCoord[i];

        for (int j = 0; j < MAX_DIRECTIONALLIGHTS; j++) {
            fIn_DirectionalLightSpacePositions[j] = gIn_DirectionalLightSpacePositions[i][j];
        }

        gl_Position = vec4(AxisMatrix * fIn_VoxelUnitCubeCoord, 1);

        EmitVertex();
    }

    EndPrimitive();
}
