const vec3 Ones = vec3(1, 1, 1);

vec3 GetUnitCubeCoordinates(const vec3 worldPosition, const vec3 voxelCenter, const vec3 voxelHalfDimensions) {
    return (worldPosition - voxelCenter) / voxelHalfDimensions;
}

vec3 GetNormalizedCoordinatesFromUnitCubeCoordinates(const vec3 unitCubeCoordinates) {
    return 0.5 * unitCubeCoordinates + 0.5;
}

bool IsWithinVoxelColume(const vec3 unitCubeCoordinates) {
    return all(lessThan(abs(unitCubeCoordinates), Ones));
}

ivec3 GetIntegerCoordinatesFromNormalizedTextureCoordinates(const ivec3 ImageSize, vec3 normalizedCoordinates) {
    return ivec3(ImageSize * normalizedCoordinates);
}

// source: https://rauwendaal.net/2013/02/07/glslrunningaverage/
void ImageAtomicAverageRGBA8(layout(r32ui) coherent volatile uimage3D voxels, ivec3 coord, vec3 nextVec3) {
    uint nextUint = packUnorm4x8 (vec4 (nextVec3, 1.0f / 255.0f));
    uint prevUint = 0;
    uint currUint;

    vec4 currVec4;

    vec3 average;
    uint count;

    //"Spin" while threads are trying to change the voxel
    while((currUint = imageAtomicCompSwap(voxels, coord, prevUint, nextUint)) != prevUint) {
        prevUint = currUint;                    //store packed rgb average and count
        currVec4 = unpackUnorm4x8(currUint);    //unpack stored rgb average and count

        average =      currVec4.rgb;        //extract rgb average
        count   = uint(currVec4.a * 255.0f);  //extract count

        //Compute the running average
        average = (average*count + nextVec3) / (count + 1);

        //Pack new average and incremented count back into a uint
        nextUint = packUnorm4x8(vec4(average, (count + 1) / 255.0f));
    }
}
