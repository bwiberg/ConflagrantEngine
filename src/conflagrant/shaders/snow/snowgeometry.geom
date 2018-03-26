#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 50) out;
//layout (triangle_strip, max_vertices = 20) out;

in vec3 gIn_WorldPosition[3];
in mat3 gIn_WorldTBN[3];
in vec2 gIn_TexCoord[3];

uniform float time;
uniform float timeSnowStart;

out vec3 fIn_WorldPosition;
out mat3 fIn_WorldTBN;
out vec2 fIn_TexCoord;
out float fIn_Snowiness;

#include "snow/SnowGeometryCommon.glsl"

void EmitRegularTriangles() {
    for(int i = 0; i < 3; i++) {
        // lower
        gl_Position = gl_in[i].gl_Position;

        fIn_WorldPosition = gIn_WorldPosition[i];
        fIn_WorldTBN = gIn_WorldTBN[i];
        fIn_TexCoord = gIn_TexCoord[i];
        fIn_Snowiness = 0;

        EmitVertex();
    }

    EndPrimitive();
}

void main(void) {
    float snows[3] = float[3](
        CalcSnowiness(gIn_WorldTBN[0][2]),
        CalcSnowiness(gIn_WorldTBN[1][2]),
        CalcSnowiness(gIn_WorldTBN[2][2])
    );

    if (snows[0] == 0 &&
        snows[1] == 0 &&
        snows[2] == 0) {
        EmitRegularTriangles();
        return;
    }

    float offsets[3] = float[3](
        snows[0] * SnowMaxHeight * CalcSnowNoise(gIn_WorldPosition[0], time - timeSnowStart),
        snows[1] * SnowMaxHeight * CalcSnowNoise(gIn_WorldPosition[1], time - timeSnowStart),
        snows[2] * SnowMaxHeight * CalcSnowNoise(gIn_WorldPosition[2], time - timeSnowStart)
    );

    ///////////////////
    // emit top part //
    ///////////////////

    for (int i = 0; i < 3; ++i) {
        vec4 offset = vec4(0, offsets[i], 0, 0);

        gl_Position = gl_in[i].gl_Position + offset;

        fIn_WorldPosition = gIn_WorldPosition[i] + offset.xyz;
        fIn_WorldTBN = gIn_WorldTBN[i];
        fIn_TexCoord = gIn_TexCoord[i];
        fIn_Snowiness = snows[i];

        EmitVertex();
    }

    EndPrimitive();

    ////////////////
    // emit sides //
    ////////////////

    for (int i = 0; i < 4; ++i) {
        int index = int(mod(i, 3));

        // upper
        vec4 offset = vec4(0, offsets[index], 0, 0);

        gl_Position = gl_in[index].gl_Position + offset;

        fIn_WorldPosition = gIn_WorldPosition[index] + offset.xyz;
        fIn_Snowiness = snows[index];

        EmitVertex();

        // lower
        gl_Position = gl_in[index].gl_Position;

        fIn_WorldPosition = gIn_WorldPosition[index];
        fIn_WorldTBN = gIn_WorldTBN[index];
        fIn_TexCoord = gIn_TexCoord[index];
        fIn_Snowiness = SnowinessMin;

        EmitVertex();
    }

    EndPrimitive();
}
